#define NUM_CHANNELS 6
#define PPM_PIN 9
#define FRAME_LENGTH 30000
#define PULSE_LENGTH 300

volatile uint16_t pwmValue[NUM_CHANNELS] = {1500,1500,1500,1500,1500,1500};
volatile uint32_t riseTime[NUM_CHANNELS];

volatile uint8_t lastPortD = 0;

const uint8_t inputPins[NUM_CHANNELS] = {2,3,4,5,6,7};

void setup() {
  Serial.begin(115200);
  for(int i=0;i<NUM_CHANNELS;i++){
    pinMode(inputPins[i], INPUT);
  }
  lastPortD = PIND;

  pinMode(PPM_PIN, OUTPUT);
  digitalWrite(PPM_PIN, HIGH);

  attachInterrupt(digitalPinToInterrupt(2), isr0, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), isr1, CHANGE);

  PCICR |= (1 << PCIE2);
  PCMSK2 |= 0b11110000;  // Enable D4-D7

  setupTimer1();
}

void loop() {
  for(int i=0;i<6;i++){
    Serial.print(pwmValue[i]);
    Serial.print(" ");
  }
  Serial.println();
  delay(200);
}

void isr0(){ readPWM(0); }
void isr1(){ readPWM(1); }

ISR(PCINT2_vect){
  uint8_t current = PIND;
  uint8_t changed = current ^ lastPortD;
  lastPortD = current;

  for(int i=2; i<6; i++){
    if(changed & (1 << (i+2))){
      readPWM(i);
    }
  }
}

void readPWM(uint8_t ch){
  if(digitalRead(inputPins[ch])){
    riseTime[ch] = micros();
  } else {
    pwmValue[ch] = micros() - riseTime[ch];
  }
}

void setupTimer1(){
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS11);  // prescaler 8 (0.5µs tick)
  OCR1A = 100;
  TIMSK1 |= (1 << OCIE1A);
}

ISR(TIMER1_COMPA_vect){
  static uint8_t channel = 0;
  static uint32_t frameSum = 0;

PORTB &= ~(1 << 1);  // D9 LOW (PB1)
OCR1A = PULSE_LENGTH * 2;

  if(channel < NUM_CHANNELS){
    uint16_t val = constrain(pwmValue[channel],1000,2000);
    // simple smoothing
    static uint16_t lastVal[NUM_CHANNELS] = {1500};
    val = (lastVal[channel] * 3 + val) / 4;
    lastVal[channel] = val;
    OCR1A = (val - PULSE_LENGTH) * 2;
    frameSum += val;
    channel++;
  } else {
    OCR1A = (FRAME_LENGTH - frameSum) * 2;
    frameSum = 0;
    channel = 0;
  }
}