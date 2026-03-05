/*
 * @brief Base .ino file to anchor temp_03_bare.c file
 */

extern "C" void app_main(void);

void setup() {
  app_main();
}

void loop() {
  // never runs
}