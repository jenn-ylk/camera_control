// this works but has some reliability issues where direction reverses if the encoder is turned too quickly... (states get changed out of turn, need to fix, likely by changing the state values back)
volatile int a_state = 0;
volatile int b_state = 0;
volatile uint32_t last_encoder_timestamp;

int static_variable = 0;

volatile int encoderValue = 0;


#define ENC_A 2
#define ENC_B 3

void setup() {

  Serial.begin(9600);
  Serial.println("starting");
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);

  // last_encoder_timestamp = millis();
  attachInterrupt(digitalPinToInterrupt(ENC_A), a_fall, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENC_B), b_fall, FALLING);
  Serial.println("done setup");
}


void loop() {

  // a_state = digitalRead(2);
  // b_state = digitalRead(3);
  // Serial.print("e:");
  // Serial.print(encoderValue);
  // Serial.print(",");
  // Serial.print("a:");
  // Serial.print(a_state);

  // Serial.print(",");

  // Serial.print("b:");
  // Serial.print(b_state);

  // Serial.print(",");

  // Serial.print("s:");
  // Serial.println(static_variable);

}

void a_fall() {
  Serial.println("a");
  // if (millis() - last_encoder_timestamp < 1000) {
  //   a_state = 0;
  //   b_state = 0;
  // }
  a_state = !a_state;
  if (a_state != b_state) {
    encoderValue++;
  }
  // last_encoder_timestamp = millis();
  Serial.println(encoderValue);
}

void b_fall() {
  Serial.println("b");
  // if (millis() - last_encoder_timestamp < 1000) {
  //   a_state = 0;
  //   b_state = 0;
  // }
  b_state = !b_state;
  if (a_state != b_state) {
    encoderValue--;
  }
  // last_encoder_timestamp = millis();
  Serial.println(encoderValue);
}

