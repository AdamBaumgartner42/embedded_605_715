/**
 * @brief Display a user supplied message in morse code
 * @name Adam Baumgartner
 * @date 1/25/26
 */

#define F_CPU 16000000UL // 16 MHz crystal on Arduino Mega

#include <avr/io.h>
#include <util/delay.h>

int app_main(void) {

   // Set PB7 (pin 13 LED) as an output
   DDRB |= (1 << DDB7);

   while (1) {
      // Toggle PB7 with XOR
      PORTB ^= (1 << PORTB7);

      // Delay ~500 ms
      _delay_ms(200);
   }

    return 0;
}