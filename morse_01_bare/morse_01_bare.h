/*
 * @brief Header file containing supporting data structures
 * @author Adam Baumgartner
 * @date 2/1/26
 */

#ifndef MORSE_01_BARE_H
#define MORSE_01_BARE_H

#define MAX_LEN 6
typedef struct {
  char letter;
  char pattern[MAX_LEN];
} MorseMap;

static const MorseMap morse_table[] = {
  {'A', "*-"},
  {'B', "---*"},
  {'C', "-*-*"},
  {'D', "-**"},
  {'E', "*"},
  {'F', "**-*"},
  {'G', "--*"},
  {'H', "****"},
  {'I', "**"},
  {'J', "*---"},
  {'K', "-*-"},
  {'L', "*-**"},
  {'M', "--"},
  {'N', "-*"},
  {'O', "---"},
  {'P', "*--*"},
  {'Q', "--*-"},
  {'R', "*-*"},
  {'S', "***"},
  {'T', "-"},
  {'U', "**-"},
  {'V', "***-"},
  {'W', "*--"},
  {'X', "-**-"},
  {'Y', "-*--"},
  {'Z', "--**"},
  {'1', "*----"},
  {'2', "**---"},
  {'3', "***--"},
  {'4', "****-"},
  {'5', "*****"},
  {'6', "-****"},
  {'7', "--***"},
  {'8', "---**"},
  {'9', "----*"},
  {'0', "-----"},
  {' ', " "}
};

#endif MORSE_01_H