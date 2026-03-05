/*
 * @brief Ardunio version of rpm display using Round Robin with Interrupts
 */

#define MSTOMIN 60000

volatile bool _risingEdge = false;
volatile 

uint16_t printInterval = 1000;

int16_t count = 0;
uint32_t now = 0;
uint32_t last = 0;
uint32_t timestamps[8] = {0};

void ISR_Handler(){
  _risingEdge = true;
}

void setup() {
  Serial.begin(115200);
  Serial.println("RPM, Count");

  // Configure Hardware Interrupt
  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), ISR_Handler, RISING);
}

void loop() {
  now = millis();

  if(_risingEdge) {
    _risingEdge = false;
    timestamps[count % 8] = now;
    count++;
  }

  if(now - last >= printInterval){
    last = now;

    if(count < 8) {
      rpm = 0;
    } else {
      int16_t deltaMS = timestamps[(count-1) % 8] - timestamps[count % 8];
      int16_t rpm = (count * MSTOMIN) / (4 * deltaMS);
    }

    Serial.print(rpm);
    Serial.print(", ");
    Serial.println(count);

    for(int i = 0; i <=8; i++){
      timestamps[i] = 0;
    }

  }

}
