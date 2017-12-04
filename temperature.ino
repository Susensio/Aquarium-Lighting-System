void initTempSensor() {
  sensor.begin();
  sensor.getAddress(tempDeviceAddress, 0);
  sensor.setResolution(tempDeviceAddress, 12);

  sensor.setWaitForConversion(false);
  sensor.requestTemperatures();
}

void controlTemp() {
  digitalWrite(heaterPin, getTemperatureCelsius() < tempRef );
  /*
  if ( getTemperatureCelsius() < tempRef )    // Cold
    digitalWrite(heaterPin, HIGH);
  else                                        // Hot
    digitalWrite(heaterPin, LOW);
  */
}

void updateMovAvg() {
  static unsigned long prevMillis = millis();
  if (millis() - prevMillis > 1000) {          // Every second
    prevMillis = millis();

    int temp = sensor.getTemp(tempDeviceAddress);

    sensor.requestTemperatures();

    pushMovAvg(temp);
    getMovAvg();
  }
}

void pushMovAvg(int value) {
  static byte i = 0;
  if (i == 8)
    i = 0;
  movAvg[i] = value;
  i++;
}

void getMovAvg() {
  long res = 0;
  int i;
  for (i = 0; i < 8; i++)
    res += movAvg[i];
  res = res / 8;
  tempAvg = (int)res;
}

float getTemperatureCelsius() {
  return tempAvg / 16.0;
}


void updateHistogram() {
  static unsigned long prevMillis = millis();
  if (millis() - prevMillis > (15UL * 60 * 1000) ) {      // 15 minutes
    prevMillis = millis();

    pushHistogram(tempAvg);
  }
}

// Add a value to the histogram
void pushHistogram(int value) {
  int i;
  for (i = 0; i < 79; i++)
    histogram[i] = histogram[i + 1];
  histogram[79] = value;
}

int minHistogram(byte scale) {
  byte endIndex = sizeof(histogram) / sizeof(int) - 1;
  int res = histogram[ endIndex ]; // Last value
  byte i;
  for ( i = 0; i < 20; i++ ) {
    int aux = histogram[ endIndex - i * scale];
    if ( aux != 0 && aux < res ) res = aux;        // Zero is not a valid temperature
  }
  return res;
}

int maxHistogram(byte scale) {
  byte endIndex = sizeof(histogram) / sizeof(int) - 1;
  int res = histogram[ endIndex ]; // Last value
  byte i;
  for ( i = 0; i < 20; i++ ) {
    int aux = histogram[ endIndex - i * scale];
    if ( aux > res ) res = aux;
  }
  return res;
}


void normalizeHistogram(byte scale) {
  int maxValue = maxHistogram(scale);
  int minValue = minHistogram(scale);

  byte i;
  for ( i = 0; i < 20; i++ ) {
    int value = constrain( histogram[80 - i * scale], minValue, maxValue);     // Eliminates zero degrees temperatures
    histogramPlot[20 - i] = map( value, minValue, maxValue, 0, 8 );
  }
  createCharGraph();
}

// Converts set of 5 measures (normalized) to LCD character
void histogramCreateChar(byte index) {
  clearChar(plotChar);
  int i;
  for (i = 0; i < 5; i++) {
    char aux = 8 - histogramPlot[index + i];
    while (aux < 8) {
      bitSet(plotChar[aux], 4 - i);
      aux++;
    }
  }
}

void clearChar(byte character[8]) {
  int i;
  for (i = 0; i < 8; i++)
    character[i] = 0;
}

void clearHistogram() {
  int i;
  for (i = 0; i < sizeof(histogram) / sizeof(int); i++)
    histogram[i] = 0;
}

