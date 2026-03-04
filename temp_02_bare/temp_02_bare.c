/*
 * @brief Log analog temperature Data to uing Round Robin Interrupts
 * @author Adam Baumgartner
 * @date 2/15/26
 */

#define F_CPU 16000000UL // 16 MHz crystal on Arduino Mega
#define BAUD 9600
// Clock Divider for 9600 Baud
#define UBRR_VAL ((F_CPU / 16/ BAUD) - 1) 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdlib.h>

#define ADC_REF_MV 5000.0
#define ADC_COUNTS 1024.0
#define TMP36_OFFSET_MV 500.0
#define TMP36_MV_PER_C 10.0
#define MILLIS_IN_SEC 1000

volatile bool tenSecondFlag = false;
volatile uint8_t secCount = 0;
volatile uint32_t ticks10s = 0;

static inline void uart0_putc(char c) {
    // Wait for transmit buffer empty
    while (!(UCSR0A & (1 << UDRE0))) { }
    UDR0 = (uint8_t)c;
}

static void uart0_puts(const char *s) {
    while (*s) uart0_putc(*s++);
}

static void uart0_print_u16(uint16_t v) {
    char buf[6];              // max "65535" + '\0'
    utoa(v, buf, 10);         // convert to decimal string
    uart0_puts(buf);
}

static void uart0_print_float(float v) {
  char buf[16];               // max 15 places + '\0'
  dtostrf(v, 0, 2, buf);      // convert float to string
  uart0_puts(buf);            
}

void uart0_init(void) {
  // Set baud rate
  UBRR0H = (uint8_t)(UBRR_VAL >> 8);  
  UBRR0L = (uint8_t)(UBRR_VAL);

  // Enable receiver and transmitter
  UCSR0B = (1 << RXEN0) | (1 << TXEN0);

  // Frame format: 8 data bits, no parity, 1 stop bit (8N1)
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

static void adc_init_a0(void) {
    // Set A0 to be an input pin
    DDRF &= ~(1 << DDF0);

    // Disable the digital input buffer for A0
    DIDR0 |= (1 << ADC0D);

    // Voltage Reference = 5V
    ADMUX = (1 << REFS0);

    // ADC prescaler: 128 -> 16 MHz / 128 = 125 kHz ADC clock 
    ADCSRA = (1 << ADEN) 
      | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    // Select ADC0 channel (A0)
    ADMUX = (ADMUX & 0xE0) | 0;
}

static uint16_t adc_read_a0(void) {
    // Start conversion
    ADCSRA |= (1 << ADSC);

    // Wait for completion (ADSC clears when done)
    while (ADCSRA & (1 << ADSC)) { }

    // Read result (must read ADCL first, then ADCH)
    uint8_t low  = ADCL;
    uint8_t high = ADCH;
    return (uint16_t)high << 8 | low;
}

void setupTimer1() {
  // Set up Timer 1 with interrupts disabled
  cli();  // disable interrupts

  // Clear Timer/Counter1 Control Register bits
  TCCR1A = 0;
  TCCR1B = 0;

  // Set bit to enable Clear Timer on Compare
  TCCR1B |= (1 << WGM12);   // CTC mode

  // Set prescaler to 1024
  TCCR1B |= (1 << CS12) | (1 << CS10); 

  // Tick rate = 16,000,000 / 1024 = 15,625 Hz
  OCR1A = 15624;  // 1 second tick count target

  // Enable Compare Match A interrupt
  TIMSK1 |= (1 << OCIE1A);  

  sei();  // enable interrupts
}

ISR(TIMER1_COMPA_vect) {
  // Fires ever 1 second, so 10 fires makes 10 seconds. 
  if (++secCount >= 10) {
    secCount = 0; // reset our counter of 1 second interrupts
    ticks10s++; // Increment count of 10s
    tenSecondFlag = true; // Raise Flag to print temperature
  }
}

float countsToF(int sensor_counts) {
  // Compute Fahrenheit from ADC Counts
  float sensor_mV = sensor_counts * ADC_REF_MV / ADC_COUNTS;
  float temp_C = (sensor_mV - TMP36_OFFSET_MV) / TMP36_MV_PER_C;
  float temp_F = (temp_C * 9 / 5) + 32;
  return temp_F;
}

int app_main(void) {
  // UART0
  uart0_init();
  uart0_puts("Booting...\r\n");

  // ADC0
  adc_init_a0();

  // Timer1
  uint32_t tickCopy;
  setupTimer1();
  
  // Loop
  while (1) {
    if (tenSecondFlag) {
      cli(); 
      tenSecondFlag = false; // clear interrupt flag
      tickCopy = ticks10s; // Copy to avoid shared data
      sei();
      
      long sensorValue = adc_read_a0();
      float temp_F = countsToF(sensorValue);

      uart0_print_u16(tickCopy*10UL);
      uart0_puts(", ");
      uart0_print_float(temp_F);
    
      uart0_puts("\r\n");
    }
  }
}

