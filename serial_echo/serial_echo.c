/**
 * @file serial_line_echo.c
 * @brief Echo lines over UART when newline is received
 * @author Adam Baumgartner
 */

#define F_CPU 16000000UL
#define BAUD 9600
#define UBRR_VAL ((F_CPU / 16 / BAUD) - 1)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#define BUF_SIZE 200

volatile char inputString[BUF_SIZE];
volatile uint8_t idx = 0;
volatile uint8_t stringComplete = 0;

//
// UART functions
//
void uart_init(void) {
    UBRR0H = (UBRR_VAL >> 8);
    UBRR0L = UBRR_VAL;

    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0); // Enable RX, TX, RX interrupt
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);             // 8N1
}

void uart_send(char c) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void uart_print(const char *s) {
    while (*s) uart_send(*s++);
}

//
// UART RX Interrupt (replaces serialEvent)
//
ISR(USART0_RX_vect) {
    char c = UDR0;

    if (idx < BUF_SIZE - 1) {
        inputString[idx++] = c;
    }

    if (c == '\n') {
        inputString[idx] = 0;   // null terminate
        stringComplete = 1;
    }
}

//
// Main
//
int app_main(void) {
    uart_init();
    sei();   // enable interrupts

    while (1) {
        if (stringComplete) {
            uart_print((char*)inputString);

            idx = 0;
            stringComplete = 0;
        }
    }
  return 0;  
}
