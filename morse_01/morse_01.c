/*
 * @brief Display serial input as Morse on an LED
 * @name Adam Baumgartner
 * @date 2/1/2026
 */

#define F_CPU 16000000UL // 16 MHz crystal on Arduino Mega

#include <avr/io.h>
#include <util/delay.h>

#define ARRAY_LEN(x) (sizeof(x) / sizeof(x[0]))

int morse_main(void) {

  // set PB7 (pin 13 LED) as an output
  DDRB |= (1 <<DDB7);

  // configure morse map

  while (1) {


  }

  return 0;
}