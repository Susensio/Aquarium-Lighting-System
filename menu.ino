// Main menu tree
void menuUpdate() {
  static RGBW colourRGBW;
  static HSI colourHSI;
  static Kelvin colourKelvin;
  static unsigned long prevTime;

  updateButtons();
  screenTimeOut();

  switch (menu) {

    case standBy: {
        stateMachine(hour(), 1, 1800);

        printScreen();

        if (backPressed || knobPressed) {
          setBacklight(settings.backlightMax);
          menu = screen;
          encoder.setPosition(settings.intensity);
        }
      } break;


    // Main screen:
    //   ----------------
    //    12:15 ☼ 26.3ºC
    //   Intensidad: ||||
    //   ----------------
    case screen: {                                    // Stand-by screen, intensity dial
        stateMachine(hour(), 1, 1800);

        settings.intensity = getEncoder(0, 20, true);
        led.setIntensity(settings.intensity * 255 / 20);

        printScreen();

        if (knobPressed) {
          menu = main;
          encoder.setPosition(0);
          lcd.clear();
        }
        if (backPressed) {
          menu = standBy;
          saveConfig();
          setBacklight(settings.backlightMin);
        }
      } break;


    // Main menu:
    //   ----------------
    //   Ajustes      3/7
    //   Probar color
    //   ----------------
    case main: {
        stateMachine(hour(), 1, 1800);

        int opt = getEncoder(0, 7, false);

        refresh(opt);

        lcd.setCursor(0, 0);
        lcd.print(F("Ajustes      "));
        //lcd.setCursor(13, 0);
        lcd.print(opt + 1);
        lcd.print("/8");
        lcd.setCursor(0, 1);

        switch (opt) {
          case 0:
            lcd.print(F("Demo"));
            break;
          case 1:
            lcd.print(F("Horario"));
            break;
          case 2:
            lcd.print(F("Colores"));
            break;
          case 3:
            lcd.print(F("Transiciones"));
            break;
          case 4:
            lcd.print(F("Probar color"));
            break;
          case 5:
            lcd.print(F("Pantalla LCD"));
            break;
          case 6:
            lcd.print(F("Temperaturas"));
            break;
          case 7:
            lcd.print(F("Fecha y hora"));
            break;
        }

        if (knobPressed) {
          lcd.clear();
          switch (opt) {
            case 0:
              menu = demo;
              createCharIcons();
              led.setIntensity(255);
              led.pauseFade();
              stateCopy = state;
              state = night;

              // Demo starts at (morningHour - 1)
              prevTime = millis() - (unsigned long)(settings.morningHour - 1) * 2000;
              break;
            case 1:
              menu = configTimes;
              encoder.setPosition(0);
              break;
            case 2:
              menu = configColor;
              encoder.setPosition(min(state, 5));   // Redundant state
              break;
            case 3:
              menu = configFade;
              encoder.setPosition(min(state, 5));   // Redundant state
              break;
            case 4:
              menu = testColor;
              encoder.setPosition(0);
              break;
            case 5:
              menu = configLcd;
              encoder.setPosition(0);
              break;
            case 6:
              menu = temperatures;
              normalizeHistogram(1);
              encoder.setPosition(1);
              break;
            case 7:
              menu = configRTC;
              if (year() < 2015) setTime(hour(), minute(), second(), day(), month(), 2015);
              encoder.setPosition(0);
              break;
          }
        }

        backTo(false, &menu, screen, settings.intensity);
      } break;


    // 48 seconds full LED cycle fast demostration
    //   ----------------
    //    13:26  ☼  DEMO
    //   ||||||||||
    //   ----------------
    case demo: {
        // 1 demoHour = 2 seconds
        unsigned long demoMinuteOfDay = (millis() - prevTime) * 60 / 2000 % 1440;
        unsigned long demoHour = demoMinuteOfDay / 60;
        unsigned long demoMinute = demoMinuteOfDay % 60;

        lcd.setCursor(1, 0);
        printHour(demoHour, demoMinute);

        lcd.setCursor(8, 0);
        printIcon();
        lcd.print(F("  DEMO"));
        lbgDemo.drawValue(demoMinuteOfDay, 1440);

        stateMachine(demoHour, 1500, 200);

        if (backPressed || (millis() - prevTime) > 2000UL * 24 * 3) {
          menu = main;
          encoder.setPosition(0);
          led.setIntensity( (int)settings.intensity * 255 / 20 );
          led.resumeFade();
          state = stateCopy;
          lcd.clear();
        }

        if (knobPressed) {
          menu = demoPause;
          prevTime = millis() - prevTime;      // Save actual demo time
        }

      } break;

    case demoPause: {
        lcd.setCursor(4, 1);
        if (millis() % 1000 > 500) lcd.print(F(" PAUSA "));
        else printSpaces(7);

        if (backPressed || (knobPressed)) {
          menu = demo;
          prevTime = millis() - prevTime;      // Load saved demo time
        }
      } break;


    // Changes fade times:
    //   ----------------
    //   ALBA OCASO NOCHE
    //    9     20    4
    //   ----------------
    case configTimes: {
        int opt = getEncoder(0, 2, false);

        refresh(opt);

        lcd.setCursor(0, 0);
        lcd.print(F("ALBA OCASO NOCHE"));

        printArrow(opt, 0, 0);
        lcd.print(settings.morningHour);

        printArrow(opt, 1, 6);
        lcd.print(settings.eveningHour);

        printArrow(opt, 2, 12);
        lcd.print(settings.nightHour);

        if (knobPressed) {
          switch (opt) {
            case 0:
              menu = configTimes_rise;
              encoder.setPosition(settings.morningHour);
              break;
            case 1:
              menu = configTimes_set;
              encoder.setPosition(settings.eveningHour);
              break;
            case 2:
              menu = configTimes_night;
              encoder.setPosition(settings.nightHour);
              break;
          }
        }
        backTo(false, &menu, main, 1);
      } break;

    case configTimes_rise: {
        encoderDisplayHour( &settings.morningHour , 1 );
        backTo(true, &menu, configTimes, 0);
      } break;

    case configTimes_set: {
        encoderDisplayHour( &settings.eveningHour , 7 );
        backTo(true, &menu, configTimes, 1);
      } break;

    case configTimes_night: {
        encoderDisplayHour( &settings.nightHour , 13 );
        backTo(true, &menu, configTimes, 2);
      } break;


    // Changes colors between fades:
    //   ----------------
    //   Ajustar colores
    //   Luz diurna
    //   ----------------
    case configColor: {
        stateMachine(hour(), 1, 1800);

        int opt = getEncoder(0, 5, false);

        refresh(opt);

        lcd.setCursor(0, 0);
        lcd.print(F("Ajustar colores"));

        lcd.setCursor(0, 1);
        switch (opt) {
          case 0:
            lcd.print(F("Noche"));
            break;
          case 1:
            lcd.print(F("Amanecer"));
            break;
          case 2:
            lcd.print(F("Luz diurna"));
            break;
          case 3:
            lcd.print(F("Atardecer"));
            break;
          case 4:
            lcd.print(F("Ocaso"));
            break;
          case 5:
            lcd.print(F("Luz lunar"));
            break;
        }

        if (knobPressed) {
          encoder.setPosition(0);
          lcd.clear();
          switch (opt) {
            case 0:
              menu = noche;
              break;
            case 1:
              menu = amanecer;
              break;
            case 2:
              menu = luzDiurna;
              break;
            case 3:
              menu = atardecer;
              break;
            case 4:
              menu = ocaso;
              break;
            case 5:
              menu = luzLunar;
              break;
          }
        }
        backTo(false, &menu, main, 2);
      } break;

    case noche:
      configKelvin(&settings.offKelvin, configColor, 0);
      break;

    case amanecer:
      configKelvin(&settings.sunRise, configColor, 1);
      break;

    case luzDiurna:
      configKelvin(&settings.sunDay, configColor, 2);
      break;

    case atardecer:
      configKelvin(&settings.sunSet1, configColor, 3);
      break;

    case ocaso:
      configKelvin(&settings.sunSet2, configColor, 4);
      break;

    case luzLunar:
      configHSI(&settings.moonLight, configColor, 5);
      break;


    // Changes fade duration:
    //   ----------------
    //   Ocaso→Luna
    //   Tiempo: 45min
    //   ----------------
    case configFade: {
        stateMachine(hour(), 1, 1800);

        int opt = getEncoder(0, 5, false);

        refresh(opt);

        lcd.setCursor(0, 0);
        switch (opt) {
          case 0:
            lcd.print(F("Noche"));
            lcd.print((char)126);
            lcd.print(F("Amanecer"));
            break;
          case 1:
            lcd.print(F("Amanecer"));
            lcd.print((char)126);
            lcd.print(F("Sol"));
            break;
          case 2:
            lcd.print(F("Sol"));
            lcd.print((char)126);
            lcd.print(F("Atardecer"));
            break;
          case 3:
            lcd.print(F("Atardecer"));
            lcd.print((char)126);
            lcd.print(F("Ocaso"));
            break;
          case 4:
            lcd.print(F("Ocaso"));
            lcd.print((char)126);
            lcd.print(F("Luna"));
            break;
          case 5:
            lcd.print(F("Luna"));
            lcd.print((char)126);
            lcd.print(F("Noche"));
            break;
        }

        lcd.setCursor(0, 1);
        lcd.print(F("Tiempo: "));

        switch (opt) {
          case 0:
            lcd.print(settings.fadeSunRise1);
            break;
          case 1:
            lcd.print(settings.fadeSunRise2);
            break;
          case 2:
            lcd.print(settings.fadeSunSet1);
            break;
          case 3:
            lcd.print(settings.fadeSunSet2);
            break;
          case 4:
            lcd.print(settings.fadeMoonRise);
            break;
          case 5:
            lcd.print(settings.fadeMoonSet);
            break;
        }
        lcd.print(F("min"));

        if (knobPressed) {
          switch (opt) {
            case 0:
              menu = nocheFade;
              encoder.setPosition(settings.fadeSunRise1);
              break;
            case 1:
              menu = amanecerFade;
              encoder.setPosition(settings.fadeSunRise2);
              break;
            case 2:
              menu = luzDiurnaFade;
              encoder.setPosition(settings.fadeSunSet1);
              break;
            case 3:
              menu = atardecerFade;
              encoder.setPosition(settings.fadeSunSet2);
              break;
            case 4:
              menu = ocasoFade;
              encoder.setPosition(settings.fadeMoonRise);
              break;
            case 5:
              menu = luzLunarFade;
              encoder.setPosition(settings.fadeMoonSet);
              break;
          }
        }

        backTo(false, &menu, main, 3);
      } break;

    case nocheFade:
      encoderDisplayByte(&settings.fadeSunRise1, 8);
      printMin();
      backTo(true, &menu, configFade, 0);
      break;

    case amanecerFade:
      encoderDisplayByte(&settings.fadeSunRise2, 8);
      printMin();
      backTo(true, &menu, configFade, 1);
      break;

    case luzDiurnaFade:
      encoderDisplayByte(&settings.fadeSunSet1, 8);
      printMin();
      backTo(true, &menu, configFade, 2);
      break;

    case atardecerFade:
      encoderDisplayByte(&settings.fadeSunSet2, 8);
      printMin();
      backTo(true, &menu, configFade, 3);
      break;

    case ocasoFade:
      encoderDisplayByte(&settings.fadeMoonRise, 8);
      printMin();
      backTo(true, &menu, configFade, 4);
      break;

    case luzLunarFade:
      encoderDisplayByte(&settings.fadeMoonSet, 8);
      printMin();
      backTo(true, &menu, configFade, 5);
      break;


    // Test raw colors:
    //   ----------------
    //   Probar color
    //   Kelvin
    //   ----------------
    case testColor: {
        stateMachine(hour(), 1, 1800);

        int opt = getEncoder(0, 2, false);

        refresh(opt);

        lcd.setCursor(0, 0);
        lcd.print(F("Probar color"));

        lcd.setCursor(0, 1);
        switch (opt) {
          case 0:
            lcd.print(F("Kelvin"));
            break;
          case 1:
            lcd.print(F("HSI"));
            break;
          case 2:
            lcd.print(F("RGBW"));
            break;
        }

        if (knobPressed) {
          led.setIntensity(255);
          encoder.setPosition(0);
          lcd.clear();
          switch (opt) {
            case 0:
              menu = testKelvin;
              menuColourKelvin = testKelvin;
              break;
            case 1:
              menu = testHSI;
              menuColourHSI = testHSI;
              break;
            case 2:
              menu = testRGBW;
              break;
          }
        }
        backTo(false, &menu, main, 4);
      } break;

    case testKelvin: {
        configKelvin(&colourKelvin, testColor, 0);
      } break;

    case testHSI: {
        configHSI(&colourHSI, testColor, 1);
      } break;

    case testRGBW: {
        led.writeRGBW(colourRGBW);
        int opt = getEncoder(0, 3, false);

        refresh(opt);

        lcd.setCursor(1, 0);
        lcd.print(F("R   G   B   W"));

        printArrow(opt, 0, 0);
        lcd.print(colourRGBW.r);

        printArrow(opt, 1, 4);
        lcd.print(colourRGBW.g);

        printArrow(opt, 2, 8);
        lcd.print(colourRGBW.b);

        printArrow(opt, 3, 12);
        lcd.print(colourRGBW.w);

        if (knobPressed) {
          encoder.setAccel(true);
          switch (opt) {
            case 0:
              menu = testRGBW_r;
              encoder.setPosition(colourRGBW.r);
              break;
            case 1:
              menu = testRGBW_g;
              encoder.setPosition(colourRGBW.g);
              break;
            case 2:
              menu = testRGBW_b;
              encoder.setPosition(colourRGBW.b);
              break;
            case 3:
              menu = testRGBW_w;
              encoder.setPosition(colourRGBW.w);
              break;
          }
        }
        backTo(false, &menu, testColor, 2);
      } break;

    case testRGBW_r: {
        led.writeRGBW(colourRGBW);
        encoderDisplayByte( &colourRGBW.r , 1 );
        backTo(true, &menu, testRGBW, 0);
      } break;

    case testRGBW_g: {
        led.writeRGBW(colourRGBW);
        encoderDisplayByte( &colourRGBW.g , 5 );
        backTo(true, &menu, testRGBW, 1);
      } break;

    case testRGBW_b: {
        led.writeRGBW(colourRGBW);
        encoderDisplayByte( &colourRGBW.b , 9 );
        backTo(true, &menu, testRGBW, 2);
      } break;

    case testRGBW_w: {
        led.writeRGBW(colourRGBW);
        encoderDisplayByte( &colourRGBW.w , 13 );
        backTo(true, &menu, testRGBW, 3);
      } break;


    // LCD settings:
    //   ----------------
    //    Min Max  Espera
    //     5  240    60s
    //   ----------------
    case configLcd: {
        stateMachine(hour(), 1, 1800);

        int opt = getEncoder(0, 2, false);

        refresh(opt);

        lcd.setCursor(1, 0);
        lcd.print(F("Min Max  Espera"));

        printArrow(opt, 0, 0);
        lcd.print(settings.backlightMin);

        printArrow(opt, 1, 4);
        lcd.print(settings.backlightMax);

        printArrow(opt, 2, 10);
        lcd.print(settings.timeOut);
        lcd.print("s");

        if (knobPressed) {
          encoder.setAccel(true);
          switch (opt) {
            case 0:
              menu = configLcd_min;
              encoder.setPosition(settings.backlightMin);
              break;
            case 1:
              menu = configLcd_max;
              encoder.setPosition(settings.backlightMax);
              break;
            case 2:
              menu = configLcd_to;
              encoder.setPosition(settings.timeOut);
              break;
          }
        }
        backTo(false, &menu, main, 5);
      } break;

    case configLcd_min: {
        setBacklight(settings.backlightMin);
        encoderDisplayByte( &settings.backlightMin , 1 );
        backTo(true, &menu, configLcd, 0);
      } break;

    case configLcd_max: {
        setBacklight(settings.backlightMax);
        encoderDisplayByte( &settings.backlightMax , 5 );
        backTo(true, &menu, configLcd, 1);
      } break;

    case configLcd_to: {
        int opt = getEncoder(5, 999, true);
        settings.timeOut = opt;
        static int prevOpt = opt;
        if (prevOpt != opt) {
          lcd.setCursor(11, 1);
          printSpaces(4);        // Erase previous number
          prevOpt = opt;
        }
        lcd.setCursor(10, 1);
        printFlash();
        lcd.print(opt);
        lcd.print("s");

        backTo(true, &menu, configLcd, 2);
      } break;


    // Temperature information and histogram:
    //   ----------------
    //   ↑27.5º▒▒26.2ºC
    //   ↓23.1º  10h
    //   ----------------
    case temperatures: {
        stateMachine(hour(), 1, 1800);

        int opt = getEncoder(1, 4, true);

        normalizeHistogram(opt);

        lcd.setCursor(10, 0);
        printTemp();

        lcd.setCursor(6, 0);
        printPlot();

        lcd.setCursor(0, 0);
        lcd.write(3);        // Max temp icon
        lcd.print( maxHistogram(opt) / 16.0, 1);
        lcd.print((char)223);
        lcd.setCursor(0, 1);
        lcd.write(2);        // Min temp icon
        lcd.print( minHistogram(opt) / 16.0, 1);
        lcd.print((char)223);

        lcd.setCursor(7, 1);
        printFlash();
        lcd.print(opt * 5);
        lcd.print(F("h "));

        if (knobPressed) {
          menu = setTemperatures;
          encoder.setPosition(settings.tempBase * 10);
        }

        backTo(false, &menu, main, 6);
      } break;

    case setTemperatures: {
        int opt = getEncoder(200, 350, true);

        settings.tempBase = opt / 10.0;

        lcd.setCursor(9, 0);
        printFlash();
        lcd.print( settings.tempBase, 1);

        backTo(true, &menu, temperatures, 1);
      } break;



    // System date and time:
    //   ----------------
    //    Fecha
    //    18/04/2015
    //   ----------------
    case configRTC: {
        stateMachine(hour(), 1, 1800);

        int opt = getEncoder(0, 2, false);

        refresh(opt);

        lcd.setCursor(1, 0);
        switch (opt) {
          case 0:
            lcd.print(F("Hora"));
            break;
          case 1:
            lcd.print(F("Fecha"));
            break;
          case 2:
            lcd.print(F("DST"));
            break;
        }

        lcd.setCursor(2, 1);
        switch (opt) {
          case 0:
            printHour(hour(), minute());
            break;
          case 1:
            printDate(day(), month(), year());
            break;
          case 2:
            if (settings.dst) lcd.print(F("Verano  "));
            else lcd.print(F("Invierno"));
            break;
        }

        if (knobPressed) {
          switch (opt) {
            case 0:
              encoder.setAccel(true);
              menu = configRTC_time;
              encoder.setPosition(0);
              prevOptTime = 0;
              configT = now();
              break;
            case 1:
              encoder.setAccel(true);
              menu = configRTC_date;
              encoder.setPosition(0);
              prevOptTime = 0;
              configT = now();
              break;
            case 2:
              settings.dst = !settings.dst;
              break;
          }
        }
        backTo(false, &menu, main, 7);
      } break;


    case configRTC_time: {
        int opt = encoder.getPosition();
        if (prevOptTime != opt) {
          long adj = (long)(opt - prevOptTime) * 60;
          configT += adj;
          prevOptTime = opt;
        }

        lcd.setCursor(1, 1);
        printFlash();
        printHour(hour(configT), minute(configT));

        backTo(true, &menu, configRTC, 0);
      } break;

    case configRTC_date: {
        int opt = encoder.getPosition();
        if (prevOptTime != opt) {
          long adj = (long)(opt - prevOptTime) * 86400;
          configT += adj;
          prevOptTime = opt;
        }

        lcd.setCursor(1, 1);
        printFlash();
        printDate(day(configT), month(configT), year(configT));

        backTo(true, &menu, configRTC, 1);
      } break;
  }
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////



// Configure Kelvin:
//   ----------------
//     TEMP     IN
//     6600K    255
//   ----------------
void configKelvin(Kelvin *colour, Menu menuBack, byte enc) {
  switch (menuColourKelvin) {
    case testKelvin: {
        led.setKelvin(*colour);
        int opt = getEncoder(0, 1, false);

        refresh(opt);

        lcd.setCursor(2, 0);
        lcd.print(F("TEMP     IN"));

        printArrow(opt, 0, 1);
        lcd.print(colour->t, 0);
        lcd.print(" K");

        printArrow(opt, 1, 10);
        lcd.print(colour->i);


        if (knobPressed) {
          encoder.setAccel(true);
          switch (opt) {
            case 0:
              menuColourKelvin = testKelvin_t;
              encoder.setPosition(colour->t / 50);
              break;
            case 1:
              menuColourKelvin = testKelvin_i;
              encoder.setPosition((int)colour->i);
              break;
          }
        }
        backTo(false, &menu, menuBack, enc);
      } break;
    case testKelvin_t: {
        led.setKelvin(*colour);
        int opt = getEncoder(0, 800, true);
        colour->t = opt * 50.0;
        static int prevOpt = opt;
        if (prevOpt != opt) {
          lcd.setCursor(2, 1);
          printSpaces(7);        // Erase previous number
          prevOpt = opt;
        }
        lcd.setCursor(1, 1);
        printFlash();
        lcd.print(colour->t, 0);
        lcd.print(" K");

        backTo(true, &menuColourKelvin, testKelvin, 0);
      } break;
    case testKelvin_i: {
        led.setKelvin(*colour);
        encoderDisplayByte( &colour->i , 11 );

        backTo(true, &menuColourKelvin, testKelvin, 1);
      } break;
  }
}

// Configure HSI:
//   ----------------
//    HUE   SAT   IN
//    -120  1.00  255
//   ----------------
void configHSI(HSI *colour, Menu menuBack, byte enc) {
  switch (menuColourHSI) {
    case testHSI: {
        led.setHSI(*colour);
        int opt = getEncoder(0, 2, false);

        refresh(opt);

        lcd.setCursor(1, 0);
        lcd.print(F("HUE   SAT   IN"));

        printArrow(opt, 0, 0);
        lcd.print(colour->h, 0);

        printArrow(opt, 1, 6);
        lcd.print(colour->s);

        printArrow(opt, 2, 12);
        lcd.print(colour->i);


        if (knobPressed) {
          encoder.setAccel(true);
          switch (opt) {
            case 0:
              menuColourHSI = testHSI_h;
              encoder.setPosition(colour->h);
              break;
            case 1:
              menuColourHSI = testHSI_s;
              encoder.setPosition(colour->s * 100);
              break;
            case 2:
              menuColourHSI = testHSI_i;
              encoder.setPosition(colour->i);
              break;
          }
        }

        backTo(false, &menu, menuBack, enc);
      } break;
    case testHSI_h: {
        led.setHSI(*colour);
        int opt = getEncoder(-359, 360, false);
        colour->h = opt;
        static int prevOpt = opt;
        if (prevOpt != opt) {
          lcd.setCursor(1, 1);
          printSpaces(4);        // Erase previous number
          prevOpt = opt;
        }
        lcd.setCursor(0, 1);
        printFlash();
        lcd.print(opt);

        backTo(true, &menuColourHSI, testHSI, 0);
      } break;
    case testHSI_s: {
        led.setHSI(*colour);
        int opt = getEncoder(0, 100, true);
        colour->s = opt / 100.0;
        static int prevOpt = opt;
        if (prevOpt != opt) {
          lcd.setCursor(7, 1);
          printSpaces(4);        // Erase previous number
          prevOpt = opt;
        }
        lcd.setCursor(6, 1);
        printFlash();
        lcd.print(colour->s);

        backTo(true, &menuColourHSI, testHSI, 1);
      } break;
    case testHSI_i: {
        led.setHSI(*colour);
        encoderDisplayByte( &colour->i , 13 );

        backTo(true, &menuColourHSI, testHSI, 2);
      } break;
  }
}


// Generic back to menu function for saving flash memory
void backTo(boolean final, Menu *mainMenu, Menu menuBack, byte enc) {
  if (backPressed || (knobPressed && final)) {
    if (!final) {
      saveConfig();
      led.setIntensity((int)settings.intensity * 255 / 20);
    }
    if (menuBack == configRTC) {
      setTime(configT);
      RTC.set(now());
    }
    *mainMenu = menuBack;
    encoder.setPosition(enc);
    encoder.setAccel(false);
    setBacklight(settings.backlightMax);
    lbgIntensity.forceRedraw();
    lbgIntensity.createAllChar();
    createCharIcons();
    lcd.clear();
  }
}

// Handles the timeout feature that changes LCD backlight
void screenTimeOut() {
  static unsigned long prevTime = millis();
  static int prevEnc = encoder.getPosition();

  // Some human interaction
  boolean interaction = knobChanged || backChanged || encoder.getPosition() != prevEnc;

  // Reset timeout
  if ( interaction ) {
    prevEnc = encoder.getPosition();
    prevTime = millis();
  }
  // Timeout. Not standBy saves EEPROM writes
  if ( !interaction && (millis() - prevTime > (unsigned long)settings.timeOut * 1000
                        && menu != standBy &&  menu != demo ) ) {

    if ( menu == demoPause ) {
      led.setIntensity( (int)settings.intensity * 255 / 20 );
      led.resumeFade();
      state = stateCopy;
    }
    menu = standBy;
    menuColourHSI = testHSI;
    menuColourKelvin = testKelvin;

    // Do NOT save config changes if timeout while in menu
    byte prevIntensity = settings.intensity;    // Don't overwrite intensity
    loadConfig();                               // Load previous settings
    settings.intensity = prevIntensity;
    saveConfig();                               // Save actual intensity

    setBacklight(settings.backlightMin);
    encoder.setPosition(settings.intensity);
    lbgIntensity.forceRedraw();
    lbgIntensity.createAllChar();
    createCharIcons();
    lcd.clear();
  }
}

void refresh(int opt) {
  static int prevOpt = opt;
  if (prevOpt != opt) {
    lcd.clear();
    prevOpt = opt;
  }
}
