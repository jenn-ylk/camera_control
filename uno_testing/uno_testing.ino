// this works but has some reliability issues where direction reverses if the encoder is turned too quickly... (states get changed out of turn, need to fix, likely by changing the state values back)
volatile int a_state = 0;
volatile int b_state = 0;
volatile uint32_t last_encoder_timestamp;

int static_variable = 0;

volatile int encoderValue = 0;


#define ENC_A 2
#define ENC_B 3

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {

  Serial.begin(9600);
  Serial.println("starting");

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  // Draw a single pixel in white
  display.drawPixel(10, 10, SSD1306_WHITE);
  display.drawPixel(20, 20, SSD1306_WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);

  display.clearDisplay();
  // Draw a single pixel in white
  display.drawPixel(20, 20, SSD1306_WHITE);
  display.drawPixel(30, 30, SSD1306_WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);

  testdrawstyles();

  // pinMode(ENC_A, INPUT_PULLUP);
  // pinMode(ENC_B, INPUT_PULLUP);

  // last_encoder_timestamp = millis();
  // attachInterrupt(digitalPinToInterrupt(ENC_A), a_fall, FALLING);
  // attachInterrupt(digitalPinToInterrupt(ENC_B), b_fall, FALLING);
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
  // if (millis() - last_encoder_timestamp < 200) {
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
  // if (millis() - last_encoder_timestamp < 200) {
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

void testdrawstyles(void) {
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Hello, world!"));

  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.println(3.141592);

  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.print(F("0x")); display.println(0xDEADBEEF, HEX);

  display.display();
}


