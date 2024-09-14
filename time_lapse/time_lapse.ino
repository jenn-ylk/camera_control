// TODO: Check compatibility with PF for alarm... 
#include <RTClib.h>
// Libraries for OLED display
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
// Make sure version number is 2.5.1 - until a fix is applied for pgmspace.h includes in the library for RP2040s
#include <Adafruit_SSD1306.h>

#define SHUTTER 18
// TODO: find out how long the shutter actually gets held for
#define SHUTTER_PERIOD 200

// Encoder Setup
volatile int a_state = 0;
volatile int b_state = 0;
#define ENC_A 16
#define ENC_B 17
#define ENC_BUTTON 19

volatile uint32_t last_encoder_timestamp;
#define RESET_STATE_DELAY 200

// OLED Display and Interface Setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3D
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Interface State and Clock
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

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display_time_oled(period_seconds);


  // Set up encoder knob and button (interrupt driven for better response)
  attachInterrupt(digitalPinToInterrupt(ENC_A), a_fall, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENC_B), b_fall, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENC_BUTTON), encoder_button, FALLING);

  // Set up the RTC
  if (!rtc.begin()) {
    Serial.println(F("Didn't find RTC :("));
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

  // Adding OLED writes seems to reduce reliability of counter state writes - potentially sharing pins, or overwriting memory?
  display_time(new_period_seconds);
  // display_time_oled(new_period_seconds);
  delay(100);


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

void display_time(int32_t period) {
  Serial.print(period / (60 * 60));
  Serial.print(F(":"));
  Serial.print((period % (60 * 60)) / 60);
  Serial.print(F(":"));
  Serial.println(period % 60);
}

void display_time_oled(int32_t period) {
  char hours = period / (60 * 60);
  char minutes = period % (60 * 60) / 60;
  char seconds = period % 60;
  display.fillRect(0, 0, 128, 32, SSD1306_BLACK);

  display.setCursor(20, 10);

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  if (hours < 10) display.print(F("0"));
  display.print(hours, DEC); 
  display.print(F(":"));
  if (minutes < 10) display.print(F("0"));
  display.print(minutes, DEC); 
  display.print(F(":"));
  if (seconds < 10) display.print(F("0"));
  display.println(seconds, DEC);

  // Show selected section
  if (timeset_mode == SECONDS) {
    display.fillRect(91, 9, 24, 16, SSD1306_INVERSE);
  } else if (timeset_mode == MINUTES) {
    display.fillRect(55, 9, 24, 16, SSD1306_INVERSE);
  } else if (timeset_mode == HOURS) {
    display.fillRect(19, 9, 24, 16, SSD1306_INVERSE);
  }

  display.display();
}

void a_fall() {
  if (millis() - last_encoder_timestamp >= RESET_STATE_DELAY) a_state = b_state = 0;
  a_state = !a_state;
  if (a_state != b_state) {
    digitalWrite(SHUTTER, HIGH);
    if (timeset_mode != SET_PERIOD) {
      new_period_seconds += pow(60, timeset_mode);
      new_period_seconds = min(new_period_seconds, 60 * 60 * 24);
    }
  }
  last_encoder_timestamp = millis();
}

void b_fall() {
  if (millis() - last_encoder_timestamp >= RESET_STATE_DELAY) a_state = b_state = 0;
  b_state = !b_state;
  if (a_state != b_state) {
    digitalWrite(SHUTTER, LOW);
    if (timeset_mode != SET_PERIOD) {
      new_period_seconds -= pow(60, timeset_mode);
      new_period_seconds = max(1, new_period_seconds);
    }
  }
  last_encoder_timestamp = millis();
}

void encoder_button() {
  timeset_mode = (timeset_mode + 1) % NUM_TIMESETTINGS;
  // TODO: extra step to either set the new time or cancel?? - see once oled stuff is in
  if (timeset_mode == SET_PERIOD) {
    period_seconds = new_period_seconds;
  }

  return;
}


