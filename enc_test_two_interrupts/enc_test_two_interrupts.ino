
volatile int a_state;
volatile int b_state;

int static_variable = 0;

volatile int encoderValue = 0;


#define ENC_A 2
#define ENC_B 3

void setup() {

  Serial.begin(9600);

  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(ENC_A), a_fall, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENC_B), b_fall, FALLING);
}


void loop() {

  a_state = digitalRead(2);
  b_state = digitalRead(3);
  Serial.print("e:");
  Serial.print(encoderValue);
  Serial.print(",");
  Serial.print("a:");
  Serial.print(a_state);

  Serial.print(",");

  Serial.print("b:");
  Serial.print(b_state);

  Serial.print(",");

  Serial.print("s:");
  Serial.println(static_variable);

}

void a_fall() {
  detachInterrupt(digitalPinToInterrupt(ENC_B));
  encoderValue++;
  attachInterrupt(digitalPinToInterrupt(ENC_A), a_rise, RISING);
  // attachInterrupt(digitalPinToInterrupt(ENC_A), a_fall, FALLING);
  // attachInterrupt(digitalPinToInterrupt(ENC_B), b_fall, FALLING);
}

void b_fall() {
  detachInterrupt(digitalPinToInterrupt(ENC_A));
  encoderValue--;
  attachInterrupt(digitalPinToInterrupt(ENC_B), b_rise, RISING);
  // attachInterrupt(digitalPinToInterrupt(ENC_A), a_fall, FALLING);
  // attachInterrupt(digitalPinToInterrupt(ENC_B), b_fall, FALLING);
}

void a_rise() {
  detachInterrupt(digitalPinToInterrupt(ENC_A));
  attachInterrupt(digitalPinToInterrupt(ENC_A), a_fall, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENC_B), b_fall, FALLING);
}

void b_rise() {
  detachInterrupt(digitalPinToInterrupt(ENC_B));
  attachInterrupt(digitalPinToInterrupt(ENC_A), a_fall, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENC_B), b_fall, FALLING);
}
