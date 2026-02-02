/*
 * @brief Display a user supplied message in morse code.
 * @author Adam Baumgartner
 * @date 2/1/26
 */

#define F_CPU 16000000UL // 16 MHz crystal on Arduino Mega
#define BAUD 9600
#define UBRR_VAL ((F_CPU / 16/ BAUD) - 1) // Clock Divider for 9600 Baud

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "morse_01_bare.h"

#define ARRAY_LEN(x) sizeof(x)/sizeof(x[0])
#define DOT_MS 132

static char buf[64];
static uint8_t idx = 0;

void uart0_init(void) {
  // Set baud rate
  UBRR0H = (uint8_t)(UBRR_VAL >> 8);  
  UBRR0L = (uint8_t)(UBRR_VAL);

  // Enable receiver and transmitter
  UCSR0B = (1 << RXEN0) | (1 << TXEN0);

  // Frame format: 8 data bits, no parity, 1 stop bit (8N1)
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void led_builtin_init(void) {
  // set PB7 as an OUTPUT
  DDRB |= (1 << DDB7); 
  // PORTB |= (1 << PB7); // drive high
  PORTB &= ~(1 << PB7); // drive low
}

int uart0_getchar(char *out) {
  if (UCSR0A & (1 << RXC0)) {
    *out = UDR0;
    return 1;
  }
  return 0;
}

void uart0_putchar(char c) {
  while (!(UCSR0A & (1 << UDRE0)));
  UDR0 = c;
}

void uart0_puts(const char *s) {
  while (*s) {
    uart0_putchar(*s++);
  }
}

char capitalize (char c) {
  if (c >= 'a' && c <= 'z') {
    c &= ~0x20; // make the capitalization bit equal to 0;
  }
  return c;
}

const char* morse_lookup(char c) {
  for (size_t i = 0; i < ARRAY_LEN(morse_table); i++) {
    if (morse_table[i].letter == c) {
      return morse_table[i].pattern; // return the matching pattern
    }
  }
  return NULL; // return null if no match
}

void dot(){
  PORTB |= (1 << PB7); // drive high
  _delay_ms(DOT_MS);
  PORTB &= ~(1 << PB7); // drive low
  _delay_ms(DOT_MS);                      
}

void dash(){
  PORTB |= (1 << PB7); // drive high
  _delay_ms(3 * DOT_MS);
  PORTB &= ~(1 << PB7); // drive low
  _delay_ms(DOT_MS);
}

void letter_gap(){
  PORTB &= ~(1 << PB7); // drive low
  _delay_ms(2 * DOT_MS);
}

void word_gap(){
  PORTB &= ~(1 << PB7); // drive low
  _delay_ms(3 * DOT_MS);
}

void flashLED(char* pattern) {
  for (int i = 0; pattern[i] != '\0'; i++) {
    char cur = pattern[i];
    char next = pattern[i+1];
    int is_last = (next == '\0') ? 1 : 0;

    if (cur == '*') {
      dot();
    } else if (cur == '-') {
      dash(); 
    } else {
      word_gap();
    }
    if (is_last) {
      letter_gap();
    }
  }
}

void charToMorse(char* buf) {
  // walk the array
  for (int i = 0; buf[i] != '\0'; i++) {
    buf[i] = capitalize(buf[i]); // capitalize lower case letters
    // Change char to morse string
    const char *pattern = morse_lookup(buf[i]);

    if (pattern) {
      flashLED(pattern);
    }
  }
}

int app_main(void) {
  
  led_builtin_init();
  uart0_init();

  while(1) {
    char c;
    if (uart0_getchar(&c)) {

      if (c == 0x1A) { // ctrl+z exit
        uart0_puts("\r\nBye!\r\n");
        break;
      }
      
      if (c == '\r' || c =='\n') {
        buf[idx] = '\0';
        uart0_puts("\r\n");
        uart0_puts(buf);
        uart0_puts("\r\n");
        _delay_ms(1000); // pause to avoid interfering with TX RX lights on board
        charToMorse(buf);

        idx = 0;
      }
      
      if (idx < (uint8_t)(ARRAY_LEN(buf) - 1)) {
        buf[idx++] = c;
      }
    }
  }
}




 

 