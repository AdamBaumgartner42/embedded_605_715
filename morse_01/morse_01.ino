/*
 * @brief Base .ino file to anchor morse_01.c file
 */

#include "morse_01.h"

// Here's the C version 
extern "C" void morse_main(void);

#define ARRAY_LEN(x) sizeof(x)/sizeof(x[0])
#define DOT_MS 132

char buf[64] = {0};
uint8_t idx = 0;

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
  digitalWrite(LED_BUILTIN, HIGH);  
  delay(DOT_MS);                      
  digitalWrite(LED_BUILTIN, LOW);   
  delay(DOT_MS);                      

}

void dash(){
  digitalWrite(LED_BUILTIN, HIGH);  
  delay(3 * DOT_MS);                      
  digitalWrite(LED_BUILTIN, LOW);   
  delay(DOT_MS);
}

void letter_gap(){
  digitalWrite(LED_BUILTIN, LOW);   
  delay(2 * DOT_MS);
}

void word_gap(){
  digitalWrite(LED_BUILTIN, LOW);   
  delay(3 * DOT_MS);
}

void flashLED(char* pattern) {
  for (int i = 0; pattern[i] != '\0'; i++) {
    char cur = pattern[i];
    char next = pattern[i+1];
    bool is_last = (next == '\0');

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

void setup() {
  // C version when ready
  // morse_main();

  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (Serial.available()){
    char c = Serial.read();
    Serial.write(c); // echo

    // when enter key is pressed
    if (c == '\r' || c == '\n') {
      buf[idx] = '\0';        // add null terminator
      Serial.write("\r\n");   // newline

      Serial.print("Morse code Displaying: ");
      Serial.println(buf);
      delay(500); // pause to avoid TX RX flashes
      charToMorse(buf);

      idx = 0;                // buffer index set to [0] for next string
    } else if (idx < ARRAY_LEN(buf) - 1) {
      buf[idx++] = c; // add char to buffer
    }
  }
}
