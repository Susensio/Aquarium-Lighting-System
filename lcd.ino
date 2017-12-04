void initLcd() {
  lcd.begin(2, 16);
  pinMode(lcdBacklightPin, OUTPUT);
  analogWrite(lcdBacklightPin, settings.backlightMax);
  createCharIcons();
  lcd.clear();
}

// Encoder sets a byte value [0..255] displayed in a certain LCD position
void encoderDisplayByte(byte * value, byte pos) {
  int opt = getEncoder(0, 255, true);
  *value = opt;
  static int prevOpt = opt;
  if (prevOpt != opt) {
    lcd.setCursor(pos, 1);
    printSpaces(3);        // Erase previous number
    prevOpt = opt;
  }
  lcd.setCursor(pos - 1, 1);
  printFlash();
  lcd.print(opt);
}

// Encoder sets a hour value [0..23] displayed in a certain LCD position
void encoderDisplayHour(byte * value, byte pos) {
  int opt = getEncoder(0, 23, false);
  *value = opt;
  static int prevOpt = opt;
  if (prevOpt != opt) {
    lcd.setCursor(pos, 1);
    printSpaces(3);        // Erase previous number
    prevOpt = opt;
  }
  lcd.setCursor(pos - 1, 1);
  printFlash();
  lcd.print(opt);
}

// Main screen:
//   ----------------
//    12:15 ☼ 26.3ºC
//   Intensidad: ||||
//   ----------------
void printScreen() {
  lcd.setCursor(1, 0);
  printHour(hour(), minute());

  lcd.setCursor(7, 0);
  printIcon();

  lcd.setCursor(10, 0);
  printTemp();

  lcd.setCursor(0, 1);
  lcd.print(F("Intensidad:"));
  lbgIntensity.drawValue(settings.intensity, 20);
}

// Formated hour
void printHour(byte h, byte m) {
  if (h < 10) printSpaces(1);
  lcd.print(h);
  if (millis() % 1000 > 500) lcd.print(":");    // Blink semicolon
  else printSpaces(1);
  if ( m < 10) lcd.print('0');
  lcd.print(m);
}

// Formated date
void printDate(byte d, byte m, int y) {
  if ( d < 10 ) lcd.print("0");
  lcd.print(d);

  lcd.print("/");

  if ( m < 10 ) lcd.print('0');
  lcd.print(m);

  lcd.print("/");

  lcd.print(y);
}

// Actual temperature
void printTemp() {
  lcd.print( getTemperatureCelsius(), 1);
  lcd.print((char)223);
  lcd.print("C");
}

// Temperature histogram plot
void printPlot() {
  lcd.write(4);
  lcd.write(5);
  lcd.write(6);
  lcd.write(7);
}

// Sunrise, sunset, moon icons
void printIcon() {
  switch (state) {
    case night:
      printSpaces(1);
      break;
    case morning:
      lcd.write(5);
      break;
    case noon:
      if (led.isFading()) lcd.write(6);
      else lcd.write(7);
      break;
    case evening:
      lcd.write(6);
      break;
    case moonrise:
      lcd.write(5);
      break;
    case moon:
      lcd.write((byte)0);
      break;
  }
}

void printArrow(int opt, int num, byte pos) {
  lcd.setCursor(pos, 1);
  if (opt == num) lcd.print((char)126);
  else printSpaces(1);
}

void printFlash() {
  if (millis() % 1000 > 500) lcd.print((char)126);   // Flashing
  else printSpaces(1);
}

// Sets LCD backlight
void setBacklight(byte value) {
  analogWrite(lcdBacklightPin, min(value, 255));
}

// Saves Sun and Moon characters in the LCD memory
void createCharIcons() {
  lcd.createChar(0, moonChar);
  lcd.createChar(5, sunRise1Char);
  lcd.createChar(6, sunRise2Char);
  lcd.createChar(7, sunChar);
}

// Saves temperature icons and graph characters in the LCD memory
void createCharGraph() {
  histogramCreateChar(0);
  lcd.createChar(4, plotChar);
  histogramCreateChar(5);
  lcd.createChar(5, plotChar);
  histogramCreateChar(10);
  lcd.createChar(6, plotChar);
  histogramCreateChar(15);
  lcd.createChar(7, plotChar);

  lcd.createChar(2, minTempChar);
  lcd.createChar(3, maxTempChar);
}

void printSpaces(char n) {
  while (n) {
    printSpaces(1);
    n--;
  }
}

void printMin(void){
  lcd.print(F("min  "));
}
