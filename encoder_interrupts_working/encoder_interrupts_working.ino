// this works but has some reliability issues where direction reverses if the encoder is turned too quickly... (states get changed out of turn, need to fix, likely by changing the state values back)
volatile int a_state = 0;
volatile int b_state = 0;
volatile uint32_t last_encoder_timestamp;

int static_variable = 0;

volatile int encoderValue = 0;


#define ENC_A 16
#define ENC_B 17
#define ENC_INDICATOR  18

// tune s.t. sharp-ish changes in direction aren't an issue too often, but importantly that slow, careful turns aren't an issue either (timing out before other pin is brushed)
#define RESET_STATE_DELAY 200

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ENC_INDICATOR, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(ENC_INDICATOR, LOW);

  last_encoder_timestamp = millis();
  attachInterrupt(digitalPinToInterrupt(ENC_A), a_fall, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENC_B), b_fall, FALLING);
  // pinMode(ENC_A, INPUT_PULLUP);
  // pinMode(ENC_B, INPUT_PULLUP);
}


void loop() {
  Serial.println(encoderValue);
}

void a_fall() {
  if (millis() - last_encoder_timestamp >= RESET_STATE_DELAY) a_state = b_state = 0;
  a_state = !a_state;
  if (a_state != b_state) {
    encoderValue++;
    digitalWrite(ENC_INDICATOR, HIGH);
  }
  last_encoder_timestamp = millis();
}

void b_fall() {
  if (millis() - last_encoder_timestamp >= RESET_STATE_DELAY) a_state = b_state = 0;
  b_state = !b_state;
  if (a_state != b_state) {
    encoderValue--;
    digitalWrite(ENC_INDICATOR, LOW);
  }
  last_encoder_timestamp = millis();
}

