#define NUM_CHANNELS 6
#define PPM_PIN 9
#define FRAME_LENGTH 21000
#define PULSE_LENGTH 300

volatile uint16_t pwmValue[NUM_CHANNELS] = {1500,1500,1500,1500,1500,1500};
volatile uint32_t riseTime[NUM_CHANNELS];

volatile uint8_t lastPortD = 0;

const uint8_t inputPins[NUM_CHANNELS] = {7,6,5,4,3,2};

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

ISR(TIMER1_COMPA_vect)
{
  static bool pulseState = false;
  static uint8_t channel = 0;
  static uint32_t sum = 0;

  if (pulseState)
  {
    // End of short LOW pulse → go HIGH
    PORTB |= (1 << 1);  // D9 HIGH
    pulseState = false;

    if (channel < NUM_CHANNELS)
    {
      uint16_t val = constrain(pwmValue[channel], 1000, 2000);

      if(val > 1495 && val < 1505)
    val = 1500;
      OCR1A = (val - PULSE_LENGTH) * 2;
      sum += val;
      channel++;
    }
    else
    {
      OCR1A = (FRAME_LENGTH - sum) * 2;
      sum = 0;
      channel = 0;
    }
  }
  else
  {
    // Start short LOW pulse
    PORTB &= ~(1 << 1);  // D9 LOW
    OCR1A = PULSE_LENGTH * 2;
    pulseState = true;
  }
}

void readPWM(uint8_t ch){
  if(digitalRead(inputPins[ch])){
    riseTime[ch] = micros();
  } else {
    uint16_t val = micros() - riseTime[ch];
    pwmValue[ch] = (pwmValue[ch] * 3 + val) / 4;
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