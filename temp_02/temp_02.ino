#define ADC_REF_MV 5000.0
#define ADC_COUNTS 1024.0
#define TMP36_OFFSET_MV 500.0
#define TMP36_MV_PER_C 10.0
#define MILLIS_IN_SEC 1000

volatile bool tenSecondFlag = false;
volatile uint8_t secCount = 0;
volatile uint32_t ticks10s = 0;

float countsToF(int sensor_counts) {
  float sensor_mV = sensor_counts * ADC_REF_MV / ADC_COUNTS;
  float temp_C = (sensor_mV - TMP36_OFFSET_MV) / TMP36_MV_PER_C;
  float temp_F = (temp_C * 9 / 5) + 32;
  return temp_F;
}

void setupTimer1() {
  cli();  // disable interrupts

  TCCR1A = 0;  // normal operation
  TCCR1B = 0;

  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= (1 << CS12) | (1 << CS10);  // prescaler 1024

  OCR1A = 15624;  // 1 second

  TIMSK1 |= (1 << OCIE1A);  // enable compare interrupt

  sei();  // enable interrupts
}

ISR(TIMER1_COMPA_vect) {
  if (++secCount >= 10) {
    secCount = 0;
    ticks10s++;
    tenSecondFlag = true;
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("Booting...");
  setupTimer1();
}

void loop() {
  if (tenSecondFlag) {
    uint32_t tickCopy;
    noInterrupts();
    tenSecondFlag = false;
    tickCopy = ticks10s;
    interrupts();

    long sensorValue = analogRead(A0);
    float temp_F = countsToF(sensorValue);

    Serial.print(tickCopy*10UL);
    Serial.print(", ");
    Serial.println(temp_F);
  }
}

