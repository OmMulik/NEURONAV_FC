#define NUM_CHANNELS 6

volatile uint16_t pwmValue[NUM_CHANNELS] = {1500,1500,1500,1500,1500,1500};
volatile uint32_t riseTime[NUM_CHANNELS];

const uint8_t pwmPins[NUM_CHANNELS] = {2,3,4,5,6,7};

void setup()
{
  Serial.begin(100000, SERIAL_8E2);   // SBUS format

  for(int i=0;i<NUM_CHANNELS;i++)
    pinMode(pwmPins[i], INPUT);

  attachInterrupt(digitalPinToInterrupt(2), isr0, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), isr1, CHANGE);

  PCICR |= (1 << PCIE2);
  PCMSK2 |= 0b11110000;
}

void loop()
{
  sendSBUS();
  delay(9);     // ~100Hz update
}



void isr0(){ readPWM(0); }
void isr1(){ readPWM(1); }

ISR(PCINT2_vect)
{
  readPWM(2);
  readPWM(3);
  readPWM(4);
  readPWM(5);
}

void readPWM(uint8_t ch)
{
  if(digitalRead(pwmPins[ch]))
    riseTime[ch] = micros();
  else
    pwmValue[ch] = micros() - riseTime[ch];
    Serial.println(pwmValue[ch]);
}

void sendSBUS()
{
  uint16_t ch[16];

  for(int i=0;i<16;i++)
  {
    if(i < NUM_CHANNELS)
    {
      uint16_t val = constrain(pwmValue[i],1000,2000);
      ch[i] = map(val,1000,2000,172,1811);
    }
    else
      ch[i] = 992;
  }

  uint8_t sbus[25];
  memset(sbus,0,25);

  sbus[0] = 0x0F;

  sbus[1]  = (ch[0] & 0xFF);
  sbus[2]  = ((ch[0] >> 8) | (ch[1] << 3));
  sbus[3]  = ((ch[1] >> 5) | (ch[2] << 6));
  sbus[4]  = (ch[2] >> 2);
  sbus[5]  = ((ch[2] >> 10) | (ch[3] << 1));
  sbus[6]  = ((ch[3] >> 7) | (ch[4] << 4));
  sbus[7]  = ((ch[4] >> 4) | (ch[5] << 7));
  sbus[8]  = (ch[5] >> 1);
  sbus[9]  = ((ch[5] >> 9) | (ch[6] << 2));
  sbus[10] = ((ch[6] >> 6) | (ch[7] << 5));
  sbus[11] = (ch[7] >> 3);
  sbus[12] = (ch[8]);
  sbus[13] = ((ch[8] >> 8) | (ch[9] << 3));
  sbus[14] = ((ch[9] >> 5) | (ch[10] << 6));
  sbus[15] = (ch[10] >> 2);
  sbus[16] = ((ch[10] >> 10) | (ch[11] << 1));
  sbus[17] = ((ch[11] >> 7) | (ch[12] << 4));
  sbus[18] = ((ch[12] >> 4) | (ch[13] << 7));
  sbus[19] = (ch[13] >> 1);
  sbus[20] = ((ch[13] >> 9) | (ch[14] << 2));
  sbus[21] = ((ch[14] >> 6) | (ch[15] << 5));
  sbus[22] = (ch[15] >> 3);

  sbus[23] = 0x00;
  sbus[24] = 0x00;

  Serial.write(sbus,25);
}