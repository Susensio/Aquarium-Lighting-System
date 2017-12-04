// Initializes LED ilumination state
void initStateMachine() {
  byte hourNow = hour();
  if ( hourNow > settings.morningHour && hourNow < settings.eveningHour) {
    state = noon;
  } else {
    if ( settings.eveningHour < settings.nightHour ) {    // Night time before 00:00
      if ( hourNow > settings.eveningHour && hourNow < settings.nightHour )
        state = moon;
      else
        state = night;
    } else {                                            // Night time adter 00:00
      if ( hourNow > settings.eveningHour || hourNow < settings.nightHour )
        state = moon;
      else
        state = night;
    }
  }
}

// Manages the scheluded LED ilumination along the day
void stateMachine(byte time, unsigned int scale, unsigned int steps) {

  switch (state) {

    case night:
      tempRef = settings.tempBase - 0.3;
      if (led.isFading())
        led.updateFade();
      else
        led.setKelvin(settings.offKelvin);

      if (time == settings.morningHour) {
        state = morning;
        led.fadeKelvin(settings.offKelvin, settings.sunRise, (unsigned int)settings.fadeSunRise1 * 60 / scale, steps);
      }
      break;

    case morning:
      tempRef = settings.tempBase + 0.0;
      led.updateFade();
      if (!led.isFading()) {
        state = noon;
        led.fadeKelvin(settings.sunRise, settings.sunDay, (unsigned int)settings.fadeSunRise2 * 60 / scale, steps);
      }
      break;

    case noon:
      tempRef = settings.tempBase + 0.3;
      if (led.isFading())
        led.updateFade();
      else
        led.setKelvin(settings.sunDay);

      if (time == settings.eveningHour) {
        state = evening;
        led.fadeKelvin(settings.sunDay, settings.sunSet1, (unsigned int)settings.fadeSunSet1 * 60 / scale, steps);
      }
      break;

    case evening:
      tempRef = settings.tempBase + 0.2;
      led.updateFade();
      if (!led.isFading()) {
        state = moonrise;
        led.fadeKelvin(settings.sunSet1, settings.sunSet2, (unsigned int)settings.fadeSunSet2 * 60 / scale, steps);
      }
      break;

    case moonrise:
      tempRef = settings.tempBase + 0.0;
      led.updateFade();
      if (!led.isFading()) {
        state = moon;
        led.fadeHSI(RGBWLed::KtoHSI(settings.sunSet2), settings.moonLight, (unsigned int)settings.fadeMoonRise * 60 / scale, steps);
      }
      break;

    case moon:
      tempRef = settings.tempBase - 0.2;
      if (led.isFading())
        led.updateFade();
      else
        led.setHSI(settings.moonLight);

      if (time == settings.nightHour) {
        state = night;
        HSI offHSI = {settings.moonLight.h, settings.moonLight.s, 0}; // Fade to black
        led.fadeHSI(settings.moonLight, offHSI, settings.fadeMoonSet * 60 / scale, steps);
      }
      break;
  }
}

// Mini state machine,  corrects daylight savings time
void dstFix() {
  if (settings.dst) {                         // Summer period (DST ON)
    if ( month() == 10 &&
         day() == (31 - (5 * year() / 4 + 1) % 7) &&
         hour() == 6 ) {                      // Last Sunday of October
      addHour(-1);
      settings.dst = false;
      saveConfig();
    }
  } else {                                    // Winter period (DST OFF)
    if ( month() == 3 &&
         day() == (31 - (5 * year() / 4 + 4) % 7) &&
         hour() == 5 ) {                      // Last Sunday of March
      addHour(1);
      settings.dst = true;
      saveConfig();
    }
  }
}

void addHour(char value) {
  tmElements_t tm;
  breakTime(now(), tm);                   // Save actual time
  tm.Hour += value;                       // Clock adjust
  RTC.write(tm);
}
