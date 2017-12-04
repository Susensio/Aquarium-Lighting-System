// Interrupt methods for handling Rotatory Encoder
void initEncoderInterrupt() {
  PCICR |= (1 << PCIE1);    // This enables Pin Change Interrupt 1 that covers the Analog input pins or Port C.
  PCMSK1 |= (1 << PCINT10) | (1 << PCINT11);  // This enables the interrupt for pin A2 and A3.
}

// Interrupt routine
ISR(PCINT1_vect) {
  encoder.tick(); // just call tick() to check the state.
}

// Initializes back and knob buttons
void initButtons() {
  pinMode(knobPin, INPUT);
  knob.attach(knobPin);
  knob.interval(5); // interval in ms

  pinMode(backPin, INPUT_PULLUP);
  back.attach(backPin);
  back.interval(5);
}

// Updates buttons momentary state (just pressed, just released)
void updateButtons() {
  knobChanged = knob.update();
  knobPressed = knobChanged && !knob.read();    // Inverted momentary switch
  knobReleased = knobChanged && knob.read();
  backChanged = back.update();
  backPressed = backChanged && !back.read();    // Inverted momentary switch
  backReleased = backChanged && back.read();
}

// Gets an encoder value within some range. Hold = saturate values | cycle them
int getEncoder(int pMin, int pMax, boolean hold) {
  int res = encoder.getPosition();
  if (hold) {                        // Saturate values
    if (res < pMin ) {
      res = pMin;
      encoder.setPosition(pMin);
    }
    if (res > pMax ) {
      res = pMax;
      encoder.setPosition(pMax);
    }
  } else {                          // Cycle values
    if (res < pMin ) {
      res = pMax;
      encoder.setPosition(pMax);
    }
    if (res > pMax ) {
      res = pMin;
      encoder.setPosition(pMin);
    }
  }
  return res;
}
