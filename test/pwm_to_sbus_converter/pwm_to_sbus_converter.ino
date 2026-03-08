#define CHANNELS 6
#define PPM_PIN 4
#define FRAME_LENGTH 20000   // 20ms frame
#define PULSE_LENGTH 300     // 300us separator pulse

// PWM input pins from receiver
int pwmPins[CHANNELS] = {32, 33, 25, 26, 27, 14};

// Storage variables
volatile uint32_t riseTime[CHANNELS];
volatile uint16_t channelValue[CHANNELS];

// Function declaration
void IRAM_ATTR handlePWM(uint8_t ch);

// ISR wrappers
void IRAM_ATTR pwmISR0(){ handlePWM(0); }
void IRAM_ATTR pwmISR1(){ handlePWM(1); }
void IRAM_ATTR pwmISR2(){ handlePWM(2); }
void IRAM_ATTR pwmISR3(){ handlePWM(3); }
void IRAM_ATTR pwmISR4(){ handlePWM(4); }
void IRAM_ATTR pwmISR5(){ handlePWM(5); }

// PWM pulse measurement



void IRAM_ATTR handlePWM(uint8_t ch)
{
  if (digitalRead(pwmPins[ch]) == HIGH)
  {
    riseTime[ch] = micros();
  }
  else
  {
    uint32_t width = micros() - riseTime[ch];

    if (width >= 900 && width <= 2100)
      channelValue[ch] = width;
  }
}

// Timer objects
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

volatile uint8_t currentChannel = 0;
volatile uint32_t frameSum = 0;

// PPM generation
void IRAM_ATTR onTimer()
{
  portENTER_CRITICAL_ISR(&timerMux);

  digitalWrite(PPM_PIN, LOW);
  delayMicroseconds(PULSE_LENGTH);
  digitalWrite(PPM_PIN, HIGH);

  if (currentChannel < CHANNELS)
  {
    uint16_t pulse = channelValue[currentChannel];
    delayMicroseconds(pulse - PULSE_LENGTH);
    frameSum += pulse;
    currentChannel++;
  }
  else
  {
    delayMicroseconds(FRAME_LENGTH - frameSum);
    frameSum = 0;
    currentChannel = 0;
  }

  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup()
{
  Serial.begin(115200);

  // Initialize PWM inputs
  for (int i = 0; i < CHANNELS; i++)
  {
    pinMode(pwmPins[i], INPUT);
    channelValue[i] = 1500; // default center value
  }

  pinMode(PPM_PIN, OUTPUT);
  digitalWrite(PPM_PIN, HIGH);

  // Attach interrupts for PWM capture
  attachInterrupt(digitalPinToInterrupt(32), pwmISR0, CHANGE);
  attachInterrupt(digitalPinToInterrupt(33), pwmISR1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(25), pwmISR2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(26), pwmISR3, CHANGE);
  attachInterrupt(digitalPinToInterrupt(27), pwmISR4, CHANGE);
  attachInterrupt(digitalPinToInterrupt(14), pwmISR5, CHANGE);

  // Timer setup
  timer = timerBegin(0, 80, true);   // 80 prescaler → 1 µs tick
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, FRAME_LENGTH, true);
  timerAlarmEnable(timer);
}

void loop()
{
  // Print channel values for debugging
  Serial.print("CH1: ");
  Serial.print(channelValue[0]);

  Serial.print("  CH2: ");
  Serial.print(channelValue[1]);

  Serial.print("  CH3: ");
  Serial.print(channelValue[2]);

  Serial.print("  CH4: ");
  Serial.print(channelValue[3]);

  Serial.print("  CH5: ");
  Serial.print(channelValue[4]);

  Serial.print("  CH6: ");
  Serial.println(channelValue[5]);

  delay(100);
}