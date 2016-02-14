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
    ISynthCore<0>::control_change(OSC_POLY_MONO, 0  );
    ISynthCore<0>::control_change(OSC_SAW_SQ   , 0  );
    ISynthCore<0>::control_change(OSC_DETUNE   , 64 );
    ISynthCore<0>::control_change(AMP_GATE_ENV , 127);
    ISynthCore<0>::control_change(LPF_CUTOFF   , 127);
    ISynthCore<0>::control_change(LPF_RESONANCE, 0  );
    ISynthCore<0>::control_change(LPF_ENV_AMT  , 0  );
    ISynthCore<0>::control_change(ENV_D_S_A    , 64 );
  }

  INLINE static void receive_midi_byte(uint8_t b) {
    ISynthCore<0>::receive_midi_byte(b);
  }

  INLINE static int8_t clock() {
    return ISynthCore<0>::clock();
  }
};
