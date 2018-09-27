// Data wire is plugged into port ?????? on the Arduino
#define ONE_WIRE_BUS 2
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensor(&oneWire);
DeviceAddress tempDeviceAddress;

const byte backPin = A0;
Bounce back = Bounce();
const byte knobPin = A1;
Bounce knob = Bounce();

boolean knobChanged;
boolean knobPressed;
boolean knobReleased;
boolean backChanged;
boolean backPressed;
boolean backReleased;

RotaryEncoder encoder(A2, A3);

const byte heaterPin = 1;

const byte lcdBacklightPin = 3;
LiquidCrystal lcd(12, 13, 8, 7, 5, 4);
//LiquidCrystal_I2C lcd(0x38);  // Set the LCD I2C address
LcdBarGraph lbgIntensity(&lcd, 4, 12, 1);
LcdBarGraph lbgDemo(&lcd, 16, 0, 1);

RGBWLed led(11, 10, 6, 9);              // (R, G, B, W) led pins


// Leds' state machine
enum State {
  night,
  morning,
  noon,
  afternoon,
  evening,
  moonrise,
  moon
};
State state;
State stateCopy;

// LCD menu state machine
enum Menu {
  standBy,
  screen,
  main,
  configColor,
  noche,
  amanecer,
  luzDiurna,
  atardecer,
  ocaso,
  luzLunar,
  configFade,
  nocheFade,
  amanecerFade,
  luzDiurnaFade,
  atardecerFade,
  ocasoFade,
  luzLunarFade,
  configTimes,
  configTimes_rise,
  configTimes_set,
  configTimes_night,
  testColor,
  configLcd,
  configLcd_min,
  configLcd_max,
  configLcd_to,
  testKelvin,
  testKelvin_t,
  testKelvin_i,
  testHSI,
  testHSI_h,
  testHSI_s,
  testHSI_i,
  testRGBW,
  testRGBW_r,
  testRGBW_g,
  testRGBW_b,
  testRGBW_w,
  temperatures,
  setTemperatures,
  demo,
  demoPause,
  configRTC,
  configRTC_date,
  configRTC_time
};
Menu menu = screen;
Menu menuColourHSI = testHSI;
Menu menuColourKelvin = testKelvin;

// Global auxiliar variables for changing time
int prevOptTime;
time_t configT;


// System settings to store in EEPROM
struct StoreStruct {

  Kelvin offKelvin;
  Kelvin sunRise;
  Kelvin sunDay;
  Kelvin sunSet1;
  Kelvin sunSet2;
  HSI moonLight;

  byte fadeSunRise1;
  byte fadeSunRise2;
  byte fadeSunSet1;
  byte fadeSunSet2;
  byte fadeMoonRise;
  byte fadeMoonSet;

  byte morningHour;
  byte eveningHour;
  byte nightHour;

  byte intensity;
  byte backlightMin;
  byte backlightMax;
  unsigned int timeOut;
  boolean dst;

  float tempBase;

  char version_of_program[4];

} settings = {     // Default values
  // FADE COLOURS
  {600, 0},            // Night
  {1000, 240},         // Sunrise
  {6100, 255},         // Sun
  {1100, 240},         // Sunset
  {700, 100},          // Dusk
  { -120, 1, 80},      // Moon

  // FADE TIMES IN MINUTES
  45,                  // Night -> Sunrise
  60,                  // Sunrise -> Sun
  60,                  // Sun -> Sunset
  45,                  // Sunset -> Dusk
  15,                  // Dusk -> Moon
  30,                  // Moon -> Night

  // FADE ACTIVATION HOUR
  9,                   // Sunrise hour
  20,                  // Sunset hour
  4,                   // Night hour

  // OTHER SETTINGS
  20,                  // Intensity
  4,                   // Minimun backlight
  255,                 // Maximun backlight
  30,                  // Timeout in seconds
  true,                // Daylight saving time

  25.0,                // Temperature reference

  // Version
  CONFIG_VERSION
};


// LCD special characters
const byte sunRise1Char[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b01110,
  0b11111,
  0b11111
};
const byte sunRise2Char[8] = {
  0b00000,
  0b00000,
  0b01110,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00000
};
const byte sunChar[8] = {
  0b01110,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00000,
  0b00000,
  0b00000
};
const byte moonChar[8] = {
  0b01000,
  0b11000,
  0b11000,
  0b11100,
  0b11100,
  0b01110,
  0b00111,
  0b00000
};
const byte maxTempChar[8] = {
  0b00100,
  0b01010,
  0b01110,
  0b01110,
  0b01110,
  0b11111,
  0b11111,
  0b01110
};
const byte minTempChar[8] = {
  0b00100,
  0b01010,
  0b01010,
  0b01010,
  0b01010,
  0b10001,
  0b11111,
  0b01110
};


// Temperature histogram variables
int movAvg[8] = {};
int tempAvg;
float tempRef;
int histogram[80] = {};
byte histogramPlot[20] = {};
byte plotChar[8];
