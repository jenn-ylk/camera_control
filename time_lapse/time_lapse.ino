// TODO: Check compatibility with PF for alarm... 
#include <RTClib.h>

#define SHUTTER 18
// TODO: find out how long the shutter actually gets held for
#define SHUTTER_PERIOD 200

#define ENC_INC 16
#define ENC_DEC 17
#define ENC_BUTTON 19

RTC_PCF8563 rtc;
uint32_t last_clock_read;
unsigned long period_seconds = 6;

void setup() {
  // put your setup code here, to run once:
  // Set up output devices (Serial, LEDs, OLED, Camera Shutter)
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SHUTTER, OUTPUT);

  // Set up encoder knob and button (interrupt driven for better response)
  pinMode(ENC_INC, INPUT_PULLUP);
  pinMode(ENC_DEC, INPUT_PULLUP);
  pinMode(ENC_BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_INC), encoder_dial, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENC_BUTTON), encoder_button, FALLING);

  // Set up the RTC
  if (!rtc.begin()) {
    Serial.println("Didn't find RTC :(");
    Serial.flush();
    while (1) delay(1000);
  }
  if (!rtc.isrunning()) {
    // start at compile/push time
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  last_clock_read = rtc.now().unixtime();
}

void loop() {
  // put your main code here, to run repeatedly:

  // Continually check the RTC time, each time the elapsed reaches the alarm period pulse XX and YY IO pins on (3.3V) (to short transistor switches to ground)
  uint32_t new_clock_read = rtc.now().unixtime();
  uint32_t elapsed = new_clock_read - last_clock_read;

  if (elapsed >= period_seconds) {
    last_clock_read = new_clock_read;

    Serial.println("Shutter, timestamp=%l", last_clock_read)
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(SHUTTER, HIGH);
    delay(SHUTTER_PERIOD);
    digitalWrite(SHUTTER, LOW);
    delay(1000 - SHUTTER_PERIOD);
    digitalWrite(LED_BUILTIN, LOW);
  }

  // Wire in the dial, one pin to button presses

}

void encoder_dial() {
  Serial.print("Encoder turned, ")
  // TODO:
  Serial.println("increasing")
  Serial.println("decreasing")
  return;
}


void encoder_button() {
  Serial.println("Encoder pressed")
  return;
}

// TODO: print current shutter period, enable increases by seconds, minutes, hours
