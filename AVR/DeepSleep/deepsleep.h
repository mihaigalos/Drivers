// References :
// http://www.home-automation-community.com/arduino-low-power-how-to-run-atmega328p-for-a-year-on-coin-cell-battery/
// http://www.gammon.com.au/power

#include <avr/sleep.h>
#include "prescaler.h"
#include "LowPower.h"

void initialDelay(){
  pinMode(1,OUTPUT);
  digitalWrite(1,LOW);
  delay(5000);
  digitalWrite(1, HIGH);
  pinMode(1,INPUT);
}

void deepsleep_200nA_noWakeup(bool has_initialdelay=true){
  if (has_initialdelay){
    initialDelay();
  } 

  for (byte i = 0; i <= A5; i++)
  {
    pinMode (i, INPUT);    
  }
    
  // disable ADC
  ADCSRA = 0;  

  set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
  noInterrupts ();           // timed sequence follows
  sleep_enable();

  // turn off brown-out enable in software
  MCUCR = bit (BODS) | bit (BODSE);
  MCUCR = bit (BODS); 
  interrupts ();             // guarantees next instruction executed
  sleep_cpu ();              // sleep within 3 clock cycles of above
}

void deepsleep_100uA_noWakeup(bool has_initialdelay=true){
  if (has_initialdelay){
    initialDelay();
  } 
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
  sleep_enable();
  sleep_cpu ();
}

void deepsleep_5uA(bool has_initialdelay=true){
  if (has_initialdelay){
    initialDelay();
  } 
  // setClockPrescaler(CLOCK_PRESCALER_1); // ignored during LowPower.powerDown since external oscillator is stopped 
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}
