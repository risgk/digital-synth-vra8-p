

A4_PITCH = 440.0

MIDI_CH             = 0
SERIAL_SPEED        = 38400
SAMPLING_RATE       = 15625
FREQUENCY_MAX       = 7812
BIT_DEPTH           = 8
NOTE_NUMBER_MIN     = 24
NOTE_NUMBER_MAX     = 84
NOTE_NUMBER_INVALID = 255

OSC_PHASE_RESOLUTION_BITS   = 24
OSC_WAVE_TABLE_AMPLITUDE    = 96
OSC_WAVE_TABLE_SAMPLES_BITS = 8
OSC_DETUNE_MUL_NUM_BITS     = 4
OSC_DETUNE_FREQ_MIN         = 64
OSC_DETUNE_FREQ_MAX         = ((252 << OSC_DETUNE_MUL_NUM_BITS) + OSC_DETUNE_FREQ_MIN) * 3
FILTER_CONTROL_INTERVAL     = 0x04
FILTER_CUTOFF_THROUGH_RATE  = 4
FILTER_TABLE_FRACTION_BITS  = 14
GATE_CONTROL_INTERVAL       = 0x04
GATE_LEVEL_STEP             = 4
ENV_GEN_CONTROL_INTERVAL    = 0x04
ENV_GEN_LEVEL_MAX           = 0xFFFF
ENV_GEN_LEVEL_A_R_STEP      = 0x1600
ENV_GEN_DECAY_FACTOR        = 248
AMP_ENV_AMT_MAX             = 240
AMP_ENV_AMT_STEP            = 16

DATA_BYTE_MAX         = 0x7F
STATUS_BYTE_INVALID   = 0x7F
DATA_BYTE_INVALID     = 0x80
STATUS_BYTE_MIN       = 0x80
NOTE_OFF              = 0x80
NOTE_ON               = 0x90
CONTROL_CHANGE        = 0xB0
SYSTEM_MESSAGE_MIN    = 0xF0
SYSTEM_EXCLUSIVE      = 0xF0
TIME_CODE             = 0xF1
SONG_POSITION         = 0xF2
SONG_SELECT           = 0xF3
TUNE_REQUEST          = 0xF6
EOX                   = 0xF7
REAL_TIME_MESSAGE_MIN = 0xF8
ACTIVE_SENSING        = 0xFE

UNISON        = 16
OSC_WAVEFORM  = 17
OSC_DETUNE    = 18
AMP_EG        = 19
FILTER_CUTOFF = 20
FILTER_RESO   = 21
FILTER_EG_AMT = 22
EG_DECAY      = 23
VELOCITY_SENS = 24
CUTOFF_V_SENS = 25
CC26_DUMMY    = 26
DETUNE_EG_AMT = 27
ALL_NOTES_OFF = 123
OMNI_MODE_OFF = 124
OMNI_MODE_ON  = 125
MONO_MODE_ON  = 126
POLY_MODE_ON  = 127

OSC_WAVEFORM_SAW   = 0
OSC_WAVEFORM_ORGAN = 64
OSC_WAVEFORM_SQ    = 127
