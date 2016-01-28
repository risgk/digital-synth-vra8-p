#pragma once

const uint8_t   MIDI_CH         = 0;
const uint16_t  SERIAL_SPEED    = 38400;
const uint16_t  SAMPLING_RATE   = 15625;
const uint16_t  FREQUENCY_MAX   = 7812;
const uint8_t   BIT_DEPTH       = 8;
const uint8_t   NOTE_NUMBER_MIN = 24;
const uint8_t   NOTE_NUMBER_MAX = 84;
const double    A4_PITCH        = 440.0;

const uint8_t   OSC_PHASE_RESOLUTION_BITS              = 16;
const uint8_t   OSC_WAVE_TABLE_AMPLITUDE               = 63;
const uint8_t   OSC_WAVE_TABLE_SAMPLES_BITS            = 8;
const uint8_t   FILTER_TABLE_FRACTION_BITS             = 14;
const uint16_t  EG_LEVEL_MAX                           = (127 << 1) << 8;
const uint8_t   EG_CONTROLLER_STEPS_BITS               = 5;
const uint8_t   EG_DECAY_RELEASE_RATE_DENOMINATOR_BITS = 8;

const uint8_t   DATA_BYTE_MAX         = 0x7F;
const uint8_t   STATUS_BYTE_INVALID   = 0x7F;
const uint8_t   DATA_BYTE_INVALID     = 0x80;
const uint8_t   STATUS_BYTE_MIN       = 0x80;
const uint8_t   NOTE_OFF              = 0x80;
const uint8_t   NOTE_ON               = 0x90;
const uint8_t   CONTROL_CHANGE        = 0xB0;
const uint8_t   SYSTEM_MESSAGE_MIN    = 0xF0;
const uint8_t   SYSTEM_EXCLUSIVE      = 0xF0;
const uint8_t   TIME_CODE             = 0xF1;
const uint8_t   SONG_POSITION         = 0xF2;
const uint8_t   SONG_SELECT           = 0xF3;
const uint8_t   TUNE_REQUEST          = 0xF6;
const uint8_t   EOX                   = 0xF7;
const uint8_t   REAL_TIME_MESSAGE_MIN = 0xF8;
const uint8_t   ACTIVE_SENSING        = 0xFE;

const uint8_t   OSC_MODE       = 16;
const uint8_t   OSC_COLOR      = 17;
const uint8_t   MOD_RATE       = 18;
const uint8_t   MOD_DEPTH      = 19;
const uint8_t   LPF_CUTOFF_ENV = 20;
const uint8_t   LPF_RESONANCE  = 21;
const uint8_t   ENV_A          = 22;
const uint8_t   ENV_D_R        = 23;
const uint8_t   ALL_NOTES_OFF  = 123;

const uint8_t   OSC_MODE_SAW       = 0;
const uint8_t   OSC_MODE_PULSE     = 16;
const uint8_t   OSC_MODE_PULSE_SAW = 32;
const uint8_t   OSC_MODE_ORGAN     = 64;
const uint8_t   OSC_MODE_BINARY    = 96;
const uint8_t   OSC_MODE_FM        = 127;
