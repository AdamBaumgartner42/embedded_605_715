/*
 * @brief Ardunio version of rpm display using Round Robin with Interrupts
 */

volatile bool flagRisingEdge = false;

volatile bool flagPrintRPM = false;

int count = 0;

void (){
  flag = true;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Booting...");
  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), ISR_handler, RISING);
}

void loop() {

  if(flagRisingEdge) {
    flagRisingEdge = false;
    Serial.println(count++);
  }
  
}
