#define PROGMEM

#include <stdint.h>

typedef signed char boolean;

inline uint8_t pgm_read_byte(const void* addr) {
  const uint8_t* p = (const uint8_t*) addr;
  return p[0];
}

inline uint16_t pgm_read_word(const void* addr) {
  // for little endian cpu
  const uint8_t* p = (const uint8_t*) addr;
  return p[0] | (p[1] << 8);
}

#include <stdio.h>
#include "./DigitalSynthVRA8P/common.h"
#include "./DigitalSynthVRA8P/synth.h"
#include "./DigitalSynthVRA8P/wav-file-out.h"

const uint16_t RECORDING_SEC = 60;

int main(int argc, char *argv[]) {
  // setup
  Synth<0>::initialize();
  FILE* bin_file = ::fopen(argv[1], "rb");
  WAVFileOut<0>::open(argv[2], RECORDING_SEC);

  // loop
  int c;
  while ((c = ::fgetc(bin_file)) != EOF) {
    Synth<0>::receive_midi_byte(c);
    uint16_t r = SAMPLING_RATE / (SERIAL_SPEED / 10);
    for (uint16_t i = 0; i < r; i++) {
      uint8_t level = Synth<0>::clock();
      WAVFileOut<0>::write(level);
    }
  }

  // teardown
  WAVFileOut<0>::close();
  ::fclose(bin_file);

  return 0;
}
