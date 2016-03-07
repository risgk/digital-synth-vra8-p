#pragma once

// #define private public  // for tests

#include "common.h"

// associations of units
#define IOsc        Osc
#define IFilter     Filter
#define IAmp        Amp
#define IGate       Gate
#define IEnvGen     EnvGen
#define IVoice      Voice
#define ISynthCore  SynthCore

#include "osc.h"
#include "filter.h"
#include "amp.h"
#include "gate.h"
#include "env-gen.h"
#include "voice.h"
#include "synth-core.h"

template <uint8_t T>
class Synth {
public:
  INLINE static void initialize() {
    ISynthCore<0>::initialize();

    // Preset
    ISynthCore<0>::control_change(UNISON       , 0  );
    ISynthCore<0>::control_change(OSC_WAVEFORM , 0  );
    ISynthCore<0>::control_change(OSC_DETUNE   , 32 );
    ISynthCore<0>::control_change(AMP_EG_AMT   , 0  );
    ISynthCore<0>::control_change(FILTER_CUTOFF, 64 );
    ISynthCore<0>::control_change(FILTER_RESO  , 64 );
    ISynthCore<0>::control_change(FILTER_EG_AMT, 96 );
    ISynthCore<0>::control_change(EG_DECAY     , 64 );
  }

  INLINE static void receive_midi_byte(uint8_t b) {
    ISynthCore<0>::receive_midi_byte(b);
  }

  INLINE static int8_t clock() {
    return ISynthCore<0>::clock();
  }
};
