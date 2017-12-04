#include <DS3232RTC.h>    //http://github.com/JChristensen/DS3232RTC
#include <Time.h>         //http://www.arduino.cc/playground/Code/Time  
#include <Wire.h>         //http://arduino.cc/en/Reference/Wire (included with Arduino IDE)
#include <EEPROM.h>

#include <RotaryEncoder.h>
#include <Bounce2.h>

#include <LiquidCrystal.h>
//#include <LiquidCrystal_I2C.h>

#include <LcdBarGraph.h>

#include <RGBWLed.h>

#include <OneWire.h>
#include <DallasTemperature.h>

////////////////////////////////////
#define DEBUG 0                  ///
#define CONFIG_VERSION "C11"     ///
////////////////////////////////////

#include "variables.h"


void setup() {
  loadConfig();

#if DEBUG
  Serial.begin(115200);
#endif

  setSyncProvider(RTC.get);   // the function to get the time from the RTC

  initStateMachine();

  initEncoderInterrupt();
  encoder.setPosition(settings.intensity);
  initButtons();

  initLcd();

  initTempSensor();
  pinMode(heaterPin, OUTPUT);
}


void loop() {
  menuUpdate();
  dstFix();
  updateMovAvg();
  controlTemp();
  updateHistogram();
}
