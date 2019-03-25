# Aquarium Lighting System

Arduino project for led aquarium lighting with lcd screen.

![alt text](http://cdn3.craftsy.com/blog/wp-content/uploads/2014/08/color-temp.jpg "Kelvin temperature scale")


### Arduino libraries

- Custom library for led interfacing [RGBWLed](https://github.com/Susensio/RGBWLed)
- Real Time Clock [DS3232RTC](http://github.com/JChristensen/DS3232RTC)
- Debouncing buttons [Bounce2](https://github.com/thomasfredericks/Bounce2)
- Dallas/Maxim 1-Wire Chips [OneWire](https://github.com/PaulStoffregen/OneWire)
- DallasTemperature
- Modified [LCDBarGraph](https://github.com/Susensio/LcdBarGraph)
- Modified [RotatoryEncoder](https://github.com/Susensio/RotaryEncoder)

### Hardware components

- Arduino UNO, ATmega328 for barebone setup
- 16 MHz Crystal Oscillator (for barebone setup)
- Real Time Clock DS3232RTC
- Temperature Sensor DS18B20
- RGB LED strip
- White LED strip
- LCD display 16x2, Hitachi HD44780 driver compatible
- Rotatory encoder and push button

### Schematics

A barebone configuration of the ATmega328 Arduino UNO microcontroller has been used. 

![alt text](schematic.png "Schematic")


## LCD menu tree

The system is highly configurable without the need for uploading a sketch with new values. Configuration preferences are saved in EPROM memory and preserved if system reboots.

Demo screen fast-forward a full day-night cycle in a minute to test colors and transitions.

A temperature dashboard has been implemented with historical data of maximum and minimum temperature and a small graph of the past days.

```
Main screen
└── Menu
    ├── Demo
    ├── Schedule
    │   ├── Sunrise
    │   ├── Sunset
    │   └── Moon
    ├── Colors
    │   ├── Night
    │   ├── Sunrise
    │   ├── Day
    │   ├── Sunset
    │   ├── Dust
    │   └── Moon
    ├── Transitions
    │   ├── Night
    │   ├── Sunrise
    │   ├── Day
    │   ├── Sunset
    │   ├── Dust
    │   └── Moon
    ├── Test color
    │   ├── Kelvin
    │   ├── HSI
    │   └── RGBW
    ├── LCD config
    │   ├── Minimum brightness 
    │   ├── Maximum brightness 
    │   └── Time out
    ├── Temperatures
    │   └── Change graph scale
    └── Date and time
```
