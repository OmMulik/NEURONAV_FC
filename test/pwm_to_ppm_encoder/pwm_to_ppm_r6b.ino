// PWM input pins (Receiver)
#define CH1  A0
#define CH2  A1
#define CH3  A2
#define CH4  A3
#define CH5  A4
#define CH6  A5

// PPM output pin
#define PPM_PIN 2

uint16_t ch[6];
uint16_t ch_filtered[6];

const uint16_t PPM_PULSE = 300;     // PPM pulse length
const uint16_t FRAME_LENGTH = 20000; // 20ms frame

void setup()
{
  pinMode(CH1, INPUT);
  pinMode(CH2, INPUT);
  pinMode(CH3, INPUT);
  pinMode(CH4, INPUT);
  pinMode(CH5, INPUT);
  pinMode(CH6, INPUT);

  pinMode(PPM_PIN, OUTPUT);
}

void loop()
{
  // Read PWM from receiver
  ch[0] = pulseIn(CH1, HIGH, 25000);
  ch[1] = pulseIn(CH2, HIGH, 25000);
  ch[2] = pulseIn(CH3, HIGH, 25000);
  ch[3] = pulseIn(CH4, HIGH, 25000);
  ch[4] = pulseIn(CH5, HIGH, 25000);
  ch[5] = pulseIn(CH6, HIGH, 25000);

  // Simple smoothing filter
  for(int i=0;i<6;i++)
  {
    ch_filtered[i] = (ch_filtered[i]*3 + ch[i]) / 4;
  }

  generatePPM();
}

void generatePPM()
{
  uint32_t frame_time = 0;

  for(int i=0;i<6;i++)
  {
    digitalWrite(PPM_PIN, HIGH);
    delayMicroseconds(PPM_PULSE);
    digitalWrite(PPM_PIN, LOW);

    delayMicroseconds(ch_filtered[i] - PPM_PULSE);
    frame_time += ch_filtered[i];
  }

  // Sync pulse
  digitalWrite(PPM_PIN, HIGH);
  delayMicroseconds(PPM_PULSE);
  digitalWrite(PPM_PIN, LOW);

  delayMicroseconds(FRAME_LENGTH - frame_time);
}