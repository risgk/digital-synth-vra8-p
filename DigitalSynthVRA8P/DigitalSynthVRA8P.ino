#include "Arduino.h"
#include "common.h"
#include "synth.h"
#include "serial-in.h"
#include "audio-out.h"

void setup() {
  Synth<0>::initialize();
  SerialIn<0>::open();
  AudioOut<0>::open();
}

void loop() {
  while (true) {
    if (SerialIn<0>::available()) {
      uint8_t b = SerialIn<0>::read();
      Synth<0>::receive_midi_byte(b);
    }
    int8_t level = Synth<0>::clock();
    AudioOut<0>::write(level);
  }
}
