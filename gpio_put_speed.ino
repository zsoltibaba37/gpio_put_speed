/**
 * @file pico_gpio_put_test.ino
 *
 * @brief Test the digital port speed, once with digitalWrite and once gpio_put
 *
 * @author Zsolt Peto
 * Contact: https://github.com/zsoltibaba37
 *
 */

float version{ 0.1 };

// ------------------ I2C Oled ------------------
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1  // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// ------------------ END I2C Oled ------------------

// ------------------ Leds ------------------
uint led1{ 14 };
uint led2{ 15 };
uint *LEDS[2]{ &led1, &led2};
bool ledState{ false };
int d{ 10 };  // delay in ms - i don't use in test phase


// ------------------ Button ------------------
uint8_t button1{ 16 };


// ------ Variables to performance measure ---------
namespace mea {

uint64_t initTime{};
double gpioResult{};
double defaultResult{};
int samples{ 200 };  // sample number - change this for exact measure

}

// --------------- predefined VOIDS ---------------
void blinkLedGpio();
void blinkLedDefault();
void resetLeds();
void displayResult();

// -------------- SETUP --------------
// -------------- SETUP --------------
void setup() {
  Serial.begin(115200);
  // ------------------ I2C Oled ------------------
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {  // Address 0x3D or 0x3C for 128x64
    Serial.println(F("-- SSD1306 allocation failed --"));
    for (;;)
      ;
  }

  // ------------------ Leds ------------------
  for (auto a : LEDS) {
    gpio_init(*a);
    gpio_set_dir(*a, GPIO_OUT);
  }

  // ------------------ Button ------------------
  gpio_init(button1);
  gpio_set_dir(button1, GPIO_IN);
  gpio_pull_up(button1);

  // --------------- Init screen ----------------
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(44, 0);
  display.println("Pico");
  display.setCursor(44, 19);
  display.println("GPIO");
  display.setCursor(44, 38);
  display.println("Test");
  display.setTextSize(1);
  display.setCursor(28, 56);
  display.println("Version: " + String(version));
  display.display();
  delay(200);
}

// -------------- LOOP --------------
// -------------- LOOP --------------
void loop() {

  if (gpio_get(button1)) {
    // -------------- Measure with gpio_put() --------------
    mea::initTime = micros();
    for (int i = 0; i < mea::samples; i++) {
      blinkLedGpio();
    }
    mea::gpioResult = (static_cast<double>(micros()) - static_cast<double>(mea::initTime));

    // -------------- Measure with digitalWrite() --------------
    mea::initTime = micros();
    for (int i = 0; i < mea::samples; i++) {
      blinkLedDefault();
    }
    mea::defaultResult = (static_cast<double>(micros()) - static_cast<double>(mea::initTime));
    
  } else { // press the button for easier reading and to stop the measure
    for (auto a : LEDS) {
      gpio_put(*a, false);
    }
    ledState = false;
  }
  // --------------- Display results ---------------
  displayResult();
}

// ------------------ VOIDS ------------------
// ------------------ VOIDS ------------------
void blinkLedGpio() {
  for (auto a : LEDS) {
    gpio_put(*a, ledState);
    //delay(d);
  }
  ledState = (ledState == true) ? false : true;
}

void blinkLedDefault() {
  for (auto a : LEDS) {
    digitalWrite(*a, ledState);
    //delay(d);
  }
  ledState = (ledState == true) ? false : true;
}

void resetLeds() {
  for (auto a : LEDS) {
    gpio_put(*a, false);
    //delay(d);
  }
  ledState = false;
}

void displayResult(){
  static const uint8_t x{5};
  static const uint8_t y{20};
  static const uint8_t incr{15};
    
  display.clearDisplay();
  display.setCursor(x+2, 4);
  display.println("digitalWrite | GPIO");
  display.setCursor(x, y);
  display.println("Res default: " + String(int(mea::defaultResult)) + " us");
  display.setCursor(x, y+incr);
  display.println("Res GPIO   : " + String(int(mea::gpioResult)) + " us");
  display.setCursor(x, y+2*incr);
  display.println("Def / GPIO : " + String(float(mea::defaultResult / mea::gpioResult)));
  display.drawRect(0, 0, 128, 64, WHITE);
  for(int i=1; i < 5; i++){
    display.drawFastHLine(0, 15*i, SCREEN_WIDTH, WHITE);
  }  
  display.display(); 
}
