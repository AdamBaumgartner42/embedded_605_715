/*
 * @brief Base .ino file to anchor morse_01_bare.c file
 */

extern "C" void app_main(void);

void setup() {
  app_main();
}

void loop() {
  // never runs
}