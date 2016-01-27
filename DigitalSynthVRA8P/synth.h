#pragma once

// #define private public  // for tests

#include "common.h"

// associations of units
#define IOsc        Osc
#define IFilter     Filter
#define IAmp        Amp
#define IEG         EG
#define IVoice      Voice
#define ISynthCore  SynthCore

#include "osc.h"
#include "filter.h"
#include "amp.h"
#include "eg.h"
#include "voice.h"
#include "synth-core.h"

template <uint8_t T>
class Synth {
public:
  INLINE static void initialize() {
    ISynthCore<0>::initialize();

    // Preset
    ISynthCore<0>::control_change(OSC_MODE      , 0  );
    ISynthCore<0>::control_change(OSC_COLOR     , 127);
    ISynthCore<0>::control_change(MOD_RATE      , 8  );
    ISynthCore<0>::control_change(MOD_DEPTH     , 127);
    ISynthCore<0>::control_change(LPF_CUTOFF_ENV, 112);
    ISynthCore<0>::control_change(LPF_RESONANCE , 64 );
    ISynthCore<0>::control_change(ENV_A         , 80 );
    ISynthCore<0>::control_change(ENV_D_R       , 72 );
  }

  INLINE static void receive_midi_byte(uint8_t b) {
    ISynthCore<0>::receive_midi_byte(b);
  }

  INLINE static int8_t clock() {
    return ISynthCore<0>::clock();
  }
};
