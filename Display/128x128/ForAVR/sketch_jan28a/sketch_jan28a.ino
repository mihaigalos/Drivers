#include "Adafruit_mfGFX.h"    // Core graphics library
#include "Adafruit_ST7735.h" // Hardware-specific library

#include "SnifferI2c.h"

#define cs   18
#define rst  13
#define dc   11

#define sda 19
#define scl 3

#define lcd_led 10
#define led 1

// following text has attribute progmem, otherwise it would use a lot of ram
static const char demoText[] = "Qu'est-ce que tu fais ce WE? ;)";
Adafruit_ST7735* tft;
SnifferI2C *sniff_i2c;

void dataReadyI2C();

void setup() {

  pinMode(lcd_led , OUTPUT);  digitalWrite(lcd_led, 1);
  pinMode(led, OUTPUT);  digitalWrite(led, 0);
  
  tft = new Adafruit_ST7735(cs, dc, sda, scl, rst); 
  tft->initR(INITR_BLACKTAB);
  
  tft->setRotation(2);
  tft->fillScreen(ST7735_BLACK);
  tft->setCursor(0, 0);
  tft->setTextColor(ST7735_WHITE);
  tft->setTextWrap(true);
  tft->print(demoText);
  
  tft->drawLine(0, 0, tft->width()-1, tft->height()-1, ST7735_YELLOW);
  tft->drawLine(tft->width()-1, 0, 0, tft->height()-1, ST7735_YELLOW);
  
  tft->drawPixel(0, tft->height()/2, ST7735_GREEN);

/*------------------------------------------------- Sniffers -------------------------------------------------*/ 
  sniff_i2c = new SnifferI2C(dataReadyI2C);
}



void loop() {

}

void dataReadyI2C(){
  
  auto count  = sniff_i2c->getCount();

  for(uint8_t i = 0; i<count;i++){
    String one (sniff_i2c->getData(i));
    tft->print(one);
  }
}



