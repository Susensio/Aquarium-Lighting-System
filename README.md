# Aquarium Lighting System

Arduino project for led aquarium lighting with lcd screen.

Sublime snippet *readme* proposes a template for `README.md` files.
Includes markdown syntax highlighter.
![alt text](http://cdn3.craftsy.com/blog/wp-content/uploads/2014/08/color-temp.jpg "Kelvin temperature scale")


## Installation

1. Download markdown files from [GitHub repository](https://github.com/Susensio/sublime-settings/)
   * Theme `ME-MonokaiC.tmTheme`
   * Settings file `Markdown.sublime-settings`
   * Snippet `Snippets\readme.sublime-snippet`
2. Copy theme and settings files into user folder (`%APPDATA%\Sublime Text 3\Packages\User\`).
2. Copy snippet file into user folder (`%APPDATA%\Sublime Text 3\Packages\User\Snippets`).

### Prerequisites

#### Arduino libraries

- Real Time Clock [DS3232RTC](http://github.com/JChristensen/DS3232RTC)
- Debouncing buttons [Bounce2](https://github.com/thomasfredericks/Bounce2)
- Dallas/Maxim 1-Wire Chips [OneWire](https://github.com/PaulStoffregen/OneWire)
- DallasTemperature
- Custom library for led interfacing [RGBWLed](https://github.com/Susensio/RGBWLed)
- Modified [LCDBarGraph](https://github.com/Susensio/LcdBarGraph)
- Modified [RotatoryEncoder](https://github.com/Susensio/RotaryEncoder)

#### Hardware components

- Real Time Clock DS3232RTC
- Temperature Sensor 

## Usage

To load the template, just type `readme` and hit TAB
As Arthur C. Clarke once said:
> Any sufficiently advanced technology is indistinguishable from magic.

You can insert code blocks:

```python
def primes_upto(max):
    factor = max*0.5 + 1
    noprimes = {j for i in range(2, factor) for j in range(i*2, max, i)}
    primes = [x for x in range(2, max) if x not in noprimes]
    return primes

print(primes(50))
```

```js
function setup() {
  createCanvas(640, 480);
}

function draw() {
  if (mouseIsPressed) {
    fill(0);
  } else {
    fill(255);
  }
  ellipse(mouseX, mouseY, 80, 80);
}
```

You can put horizontal lines...

---

and even create tables!

| Tables        | Are           | Cool  |
| ------------- |:-------------:| -----:|
| col 3 is      | right-aligned | 1600€ |
| col 2 is      | centered      |   12€ |
| zebra stripes | are neat      |    1€ |

More examples in [examples directory](examples/)

## Credits

TODO: Write credits
