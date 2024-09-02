#define ENC_A 13
#define ENC_B 14

#define ENC_PUSH 19
volatile int encoderValue = 0;

void setup() {
  Serial.begin(9600);
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  
  // starting from "phase 1"
  attachInterrupt(digitalPinToInterrupt(ENC_A), a_rise, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_B), b_rise, RISING);

  pinMode(ENC_PUSH, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_PUSH), led_off, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_PUSH), led_on, FALLING);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}
void loop() {
  Serial.println(encoderValue);
  delay(1000);
}


void led_on() {
  Serial.println("button pressed");
  digitalWrite(LED_BUILTIN, HIGH);
}

void led_off() {
  digitalWrite(LED_BUILTIN, LOW);
}

void a_fall() {
  detachInterrupt(digitalPinToInterrupt(ENC_A));
  if (digitalRead(ENC_A) == digitalRead(ENC_B)) {
    encoderValue++;
  }
  else {
    encoderValue--;
  }
  attachInterrupt(digitalPinToInterrupt(ENC_A), a_rise, RISING);
}

void a_rise() {
  detachInterrupt(digitalPinToInterrupt(ENC_A));
  if (digitalRead(ENC_A) == digitalRead(ENC_B)) {
    encoderValue++;
  }
  else {
    encoderValue--;
  }
  attachInterrupt(digitalPinToInterrupt(ENC_A), a_fall, FALLING);
}

void b_fall() {
  detachInterrupt(digitalPinToInterrupt(ENC_B));
  if (digitalRead(ENC_A) != digitalRead(ENC_B)) {
    encoderValue++;
  }
  else {
    encoderValue--;
  }
  attachInterrupt(digitalPinToInterrupt(ENC_B), b_rise, RISING);
}

void b_rise() {
  detachInterrupt(digitalPinToInterrupt(ENC_B));
  if (digitalRead(ENC_A) != digitalRead(ENC_B)) {
    encoderValue++;
  }
  else {
    encoderValue--;
  }
  attachInterrupt(digitalPinToInterrupt(ENC_B), b_fall, FALLING);
}
