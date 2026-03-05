/*
 * @brief Measure RPM with Round Robin Interrupts architecture
 * @author Adam Baumgartner
 * @date 2/22/26
 * @submittedDate 3/5/2026
 */

#define F_CPU 16000000UL // 16 MHz crystal on Arduino Mega
#define BAUD 9600
// Clock Divider for 9600 Baud
#define UBRR_VAL ((F_CPU / 16/ BAUD) - 1) 

#define MSTOMIN 60000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdlib.h>


volatile uint16_t msecCount = 0;
volatile uint32_t _millis = 0;
volatile uint32_t timestamps[8] = {0}; // Holds time during hardware interrupt

volatile bool oneSecondFlag = false;
volatile bool risingEdgeFlag = false;

int16_t count = 0;
int16_t rpm = 0;
int16_t deltaMS = 0;

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


void setupTimer1() {
  // Configure Timer 1 for 1 ms firing
  cli();  // disable interrupts

  // Clear Timer/Counter1 Control Register bits
  TCCR1A = 0;
  TCCR1B = 0;

  // Set bit to enable Clear Timer on Compare
  TCCR1B |= (1 << WGM12);   // CTC mode

  // Set prescaler to 8
  TCCR1B |= (1 << CS11); 

  // Tick rate = 16,000,000 / 8 = 2,000,000 Hz
  OCR1A = 1999;  // 1 milliSecond tick count target

  // Enable Compare Match A interrupt
  TIMSK1 |= (1 << OCIE1A);  

  sei();  // enable interrupts
}

ISR(TIMER1_COMPA_vect) {
  // Increment millis
  _millis++;

  // Fires every 1ms, so 1000 fires makes 1 seconds. 
  if (++msecCount >= 1000) {
    msecCount = 0;
    oneSecondFlag = true; // Raise Flag 
  }
}


void setupPin2Rising()
{
  cli();   

  // Configure PE4 (Arduino pin 2) as input
  DDRE &= ~(1 << PE4);

  // Configure interrupt for RISING edge
  // ISC41 = 1
  // ISC40 = 1
  EICRB |= (1 << ISC41) | (1 << ISC40);

  // Enable external interrupt INT4
  EIMSK |= (1 << INT4);

  sei();   
}

// Pin2 Rising Interrupt
ISR(INT4_vect)
{
  risingEdgeFlag = true;
}


int app_main(void) {
  // UART0
  uart0_init();
  uart0_puts("Booting...\r\n");
  uart0_puts("RPM\r\n");

  // Timers
  setupTimer1();

  // Interrupts
  setupPin2Rising();
  
  // Loop
  while (1) {

    if (risingEdgeFlag){
      risingEdgeFlag = false;
      timestamps[count % 8] = _millis;
      count++;
    }

    if (oneSecondFlag) {
      oneSecondFlag = false;
      // If less than full buffer, 0 rpm (shortcut)
      if (count < 8){
        rpm = 0;
      } else {
        // Calculate delta time start and end 
        deltaMS = timestamps[(count-1) % 8] - timestamps[count % 8];
        // Calculate RPM
        rpm = (count * MSTOMIN) / (4 * deltaMS);
      }

      // Reset count - the array index
      count = 0;
      // Print RPM
      uart0_print_u16(rpm);
      uart0_puts("\r\n");
    }
  }
}