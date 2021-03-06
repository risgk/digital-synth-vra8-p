#pragma once

const double    A4_PITCH = 440.0;

const uint8_t   MIDI_CH             = 0;
const uint16_t  SERIAL_SPEED        = 38400;
const uint16_t  SAMPLING_RATE       = 15625;
const uint16_t  FREQUENCY_MAX       = 7812;
const uint8_t   BIT_DEPTH           = 8;
const uint8_t   NOTE_NUMBER_MIN     = 24;
const uint8_t   NOTE_NUMBER_MAX     = 84;
const uint8_t   NOTE_NUMBER_INVALID = 255;

const uint8_t   OSC_PHASE_RESOLUTION_BITS   = 16;
const uint8_t   OSC_WAVE_TABLE_AMPLITUDE    = 96;
const uint8_t   OSC_WAVE_TABLE_SAMPLES_BITS = 8;
const uint8_t   OSC_DETUNE_DIV_NUM_BITS     = 3;
const uint8_t   OSC_DETUNE_FREQ_MAX         = ((127 >> OSC_DETUNE_DIV_NUM_BITS) + 1) * 3;
const uint8_t   FILTER_CONTROL_INTERVAL     = 0x04;
const uint8_t   FILTER_TABLE_FRACTION_BITS  = 14;
const uint8_t   GATE_CONTROL_INTERVAL       = 0x10;
const uint8_t   GATE_LEVEL_MAX              = 30;
const uint8_t   GATE_LEVEL_STEP             = 2;
const uint8_t   ENV_GEN_CONTROL_INTERVAL    = 0x04;
const uint8_t   ENV_GEN_LEVEL_MAX           = 252;
const uint8_t   ENV_GEN_LEVEL_A_R_STEP      = 4;
const uint8_t   ENV_GEN_DECAY_FACTOR        = 248;

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

const uint8_t   UNISON        = 16;
const uint8_t   OSC_WAVEFORM  = 17;
const uint8_t   OSC_DETUNE    = 18;
const uint8_t   AMP_EG        = 19;
const uint8_t   FILTER_CUTOFF = 20;
const uint8_t   FILTER_RESO   = 21;
const uint8_t   FILTER_EG_AMT = 22;
const uint8_t   EG_DECAY      = 23;
const uint8_t   ALL_NOTES_OFF = 123;

const uint8_t   OSC_WAVEFORM_SAW   = 0;
const uint8_t   OSC_WAVEFORM_ORGAN = 64;
const uint8_t   OSC_WAVEFORM_SQ    = 127;
