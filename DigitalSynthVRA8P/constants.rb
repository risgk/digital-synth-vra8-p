

A4_PITCH = 440.0

MIDI_CH             = 0
SERIAL_SPEED        = 38400
SAMPLING_RATE       = 15625
FREQUENCY_MAX       = 7812
BIT_DEPTH           = 8
NOTE_NUMBER_MIN     = 24
NOTE_NUMBER_MAX     = 84
NOTE_NUMBER_INVALID = 255

OSC_PHASE_RESOLUTION_BITS   = 16
OSC_WAVE_TABLE_AMPLITUDE    = 96
OSC_WAVE_TABLE_SAMPLES_BITS = 8
FILTER_TABLE_FRACTION_BITS  = 14

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
ALL_NOTES_OFF = 123

OSC_WAVEFORM_SAW   = 0
OSC_WAVEFORM_ORGAN = 64
OSC_WAVEFORM_SQ    = 127
