#include "own_voltage.h"
#include "supercapacitor.h"

void Supercapacitor::flashLed(const uint8_t led, const uint8_t times) const{
  for(uint8_t i = 0; i<times; ++i){
    digitalWrite(led, LOW);
    delay(200);
    digitalWrite(led, HIGH);
    delay(200);
  }
}

void Supercapacitor3V::voltageToLeds(){
  digitalWrite(red_led_, HIGH);
  digitalWrite(green_led_, HIGH);
  digitalWrite(blue_led_, HIGH);

    uint16_t vcc = readVcc();
    uint8_t led =-1, times=0;
    if(vcc >= 2850UL) {
      led = blue_led_; times=3;
    } else if(vcc >= 2750UL && vcc < 2850UL){
      led = blue_led_; times=2;
    } else if(vcc >= 2650UL && vcc < 2750UL){
      led = blue_led_; times=1;
    } else if(vcc >= 2550UL && vcc < 2650UL){
      led = green_led_; times=3;
    } else if(vcc >= 2450UL && vcc < 2550UL){
      led = green_led_; times=2;
    }else if(vcc >= 2350UL && vcc < 2450UL){
      led = green_led_; times=1;
    }else if(vcc >= 2250UL && vcc < 2350UL){
      led = red_led_; times=3;
    }else if(vcc >= 2150UL && vcc < 2250UL){
      led = red_led_; times=2;
    }else if(vcc < 2150UL){
      led = red_led_; times=1;
    }

    flashLed(led, times);
    delay(1000);
}

void Supercapacitor2_7V::voltageToLeds(){
  digitalWrite(red_led_, HIGH);
  digitalWrite(green_led_, HIGH);
  digitalWrite(blue_led_, HIGH);

    uint16_t vcc = readVcc();
    uint8_t led =-1, times=0;
    if(vcc >= 2600UL) {
      led = blue_led_; times=3;
    } else if(vcc >= 2525UL && vcc < 2600UL){
      led = blue_led_; times=2;
    } else if(vcc >= 2450UL && vcc < 2525UL){
      led = blue_led_; times=1;
    } else if(vcc >= 2375UL && vcc < 2450UL){
      led = green_led_; times=3;
    } else if(vcc >= 2300UL && vcc < 2375UL){
      led = green_led_; times=2;
    }else if(vcc >= 2225UL && vcc < 2300UL){
      led = green_led_; times=1;
    }else if(vcc >= 2150UL && vcc < 2225UL){
      led = red_led_; times=3;
    }else if(vcc >= 2075UL && vcc < 2150UL){
      led = red_led_; times=2;
    }else if(vcc < 2075UL){
      led = red_led_; times=1;
    }

    flashLed(led, times);
    delay(1000);
}
