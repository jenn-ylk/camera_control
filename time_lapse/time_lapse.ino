// TODO: Check compatibility with PF for alarm... 
#include <RTClib.h>

#define SHUTTER 18
// TODO: find out how long the shutter actually gets held for
#define SHUTTER_PERIOD 200

volatile int a_state = 0;
volatile int b_state = 0;
#define ENC_A 16
#define ENC_B 17
#define ENC_BUTTON 19

volatile uint32_t last_encoder_timestamp;
#define RESET_STATE_DELAY 200

enum time_setting {
  SECONDS,
  MINUTES,
  HOURS,
  SET_PERIOD,
  NUM_TIMESETTINGS,
};

RTC_PCF8563 rtc;
uint32_t last_clock_read;
volatile int32_t period_seconds = 6;
volatile int32_t new_period_seconds = period_seconds;
volatile unsigned int timeset_mode = SECONDS;

void setup() {
  // Set up output devices (Serial, LEDs, OLED, Camera Shutter)
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SHUTTER, OUTPUT);

  // Set up encoder knob and button (interrupt driven for better response)
  attachInterrupt(digitalPinToInterrupt(ENC_A), a_fall, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENC_B), b_fall, FALLING);
  // attachInterrupt(digitalPinToInterrupt(ENC_BUTTON), encoder_button, FALLING);

  // Set up the RTC
  if (!rtc.begin()) {
    Serial.println("Didn't find RTC :(");
    Serial.flush();
    while (1) delay(1000);
  }
  if (!rtc.isrunning()) rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  last_clock_read = rtc.now().unixtime();

  last_encoder_timestamp = millis();
}

void loop() {
  // Continually check the RTC time, each time the elapsed reaches the alarm period pulse XX and YY IO pins on (3.3V) (to short transistor switches to ground)
  uint32_t new_clock_read = rtc.now().unixtime();
  uint32_t elapsed = new_clock_read - last_clock_read;

  if (elapsed >= period_seconds) {
    last_clock_read = new_clock_read;

    activate_shutter();
  }

  display_time(period_seconds);
  delay(1000);


  // Wire in the dial, one pin to button presses
}


// Shutter Control
void activate_shutter() {
  // Serial.print("Shutter, timestamp=");
  // Serial.println(last_clock_read);
  digitalWrite(LED_BUILTIN, HIGH);
  // digitalWrite(SHUTTER, HIGH);
  delay(SHUTTER_PERIOD);
  // digitalWrite(SHUTTER, LOW);
  // delay(1000 - SHUTTER_PERIOD);
  digitalWrite(LED_BUILTIN, LOW);
}

void a_fall() {
  if (millis() - last_encoder_timestamp >= RESET_STATE_DELAY) a_state = b_state = 0;
  a_state = !a_state;
  if (a_state != b_state) {
    digitalWrite(SHUTTER, HIGH);
    new_period_seconds += pow(60, timeset_mode);
    new_period_seconds = min(new_period_seconds, 60 * 60 * 24);
  }
  last_encoder_timestamp = millis();
  // TODO: make this on button press
  period_seconds = new_period_seconds;
}

void b_fall() {
  if (millis() - last_encoder_timestamp >= RESET_STATE_DELAY) a_state = b_state = 0;
  b_state = !b_state;
  if (a_state != b_state) {
    digitalWrite(SHUTTER, LOW);
    new_period_seconds -= pow(60, timeset_mode);
    new_period_seconds = max(0, new_period_seconds);
  }
  last_encoder_timestamp = millis();
  // TODO: make this only on the button press
  period_seconds = new_period_seconds;
}


void display_time(uint32_t period) {
  Serial.print(period / (60 * 60));
  Serial.print(':');
  Serial.print((period % (60 * 60)) / 60);
  Serial.print(':');
  Serial.println(period % 60);
}

void encoder_button() {
  timeset_mode = (timeset_mode + 1) % NUM_TIMESETTINGS;
  // TODO: extra step to either set the new time or cancel?? - see once oled stuff is in
  if (timeset_mode == SET_PERIOD) {
    period_seconds = new_period_seconds;
    // Serial.println("Set new period:");
    // display_time(period_seconds);
  }
  // Serial.print("Encoder pressed new mode = ");
  // Serial.println(timeset_mode);

  return;
}

