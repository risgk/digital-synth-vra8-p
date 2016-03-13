# Digital Synth VRA8-P 0.0.0

- 2016-03-13 ISGK Instruments
- <https://github.com/risgk/digital-synth-vra8-p>

## Concept

- Pseudo Polyphonic Synthesizer for Arduino Uno

## Features

- 3 Voice Pseudo Polyphonic (Paraphonic) Synthesizer, MIDI Sound Module
- Serial MIDI In (38400 bps), PWM Audio Out (Pin 6), PWM Rate: 62500 Hz
    - We recommend adding a RC filter circuit to reduce PWM ripples
    - A cutoff frequency 10.6 kHz (R: 150 ohm, C: 100 nF) works well
- Sampling Rate: 15625 Hz, Bit Depth: 8 bits
- LPF Attenuation Slope: -12 dB/oct
- Recommending [Hairless MIDI<->Serial Bridge](http://projectgus.github.io/hairless-midiserial/) to connect PC
- Files
    - `DigitalSynthVRA8P.ino` is a sketch for Arduino Uno
    - `MakeSampleWavFile.cc` is for Debugging on PC
        - Requiring GCC (G++) or other
        - `make-sample-wav-file-cc.bat` makes a sample WAV file (in Windows)
    - `generate-*.rb` generate source files
        - Requiring a Ruby execution environment

## VRA8-P CTRL

- Parameter Editor (MIDI Controller) for VRA8-P, Web App
- We recommend Google Chrome, which implements Web MIDI API
- Recommending [loopMIDI](http://www.tobias-erichsen.de/software/loopmidi.html) (virtual loopback MIDI cable) to connect VRA8-P
- CAUTION: Click sounds may occur when you change the controllers (especially AMP EG and FILTER CUTOFF)
- CAUTION: Low FILTER CUTOFF with high FILTER RESO can damage the speakers

## Controllers

    +----------------+-----------------+-----------------+-----------------+----------------+
    | Controller     | Value 0         | Value 64        | Value 127       | Notes          |
    +----------------+-----------------+-----------------+-----------------+----------------+
    | UNISON         | OFF             | (ON)            | ON              |                |
    | OSC WAVEFORM   | SAW (SAWTOOTH)  | ORGAN           | SQ (SQUARE)     |                |
    | OSC DETUNE     | 0.24 Hz         | 2.1 Hz          | 3.8 Hz          |                |
    | AMP EG         | OFF             | (ON)            | ON              |                |
    | FILTER CUTOFF  | 0.22 kHz        | 1.2 kHz         | 7.0 kHz         | Range 4 ~ 124  |
    | FILTER RESO    | Q = 0.71        | Q = 1.4         | Q = 2.6         | 17 Steps       |
    | FILTER EG AMT  | -125 ~ 0        | 0               | 0 ~ +124        |                |
    | EG DECAY       | 21 ms           | 1.4 s           | 5.4 s           | Decay Time     |
    +----------------+-----------------+-----------------+-----------------+----------------+

## MIDI Implementation Chart

      [Experimental Synthesizer]                                      Date: 2016-03-13       
      Model  Digital Synth VRA8-P     MIDI Implementation Chart       Version: 0.0.0         
    +-------------------------------+---------------+---------------+-----------------------+
    | Function...                   | Transmitted   | Recognized    | Remarks               |
    +-------------------------------+---------------+---------------+-----------------------+
    | Basic        Default          | x             | 1             |                       |
    | Channel      Changed          | x             | x             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Mode         Default          | x             | Mode 3        |                       |
    |              Messages         | x             | x             |                       |
    |              Altered          | ************* |               |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Note                          | x             | 0-127         |                       |
    | Number       : True Voice     | ************* | 24-84         |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Velocity     Note ON          | x             | x             |                       |
    |              Note OFF         | x             | x             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | After        Key's            | x             | x             |                       |
    | Touch        Ch's             | x             | x             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Pitch Bend                    | x             | x             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Control                    16 | x             | o             | UNISON                |
    | Change                     17 | x             | o             | OSC WAVEFORM          |
    |                            18 | x             | o             | OSC DETUNE            |
    |                            19 | x             | o             | AMP EG                |
    |                            20 | x             | o             | FILTER CUTOFF         |
    |                            21 | x             | o             | FILTER RESO           |
    |                            22 | x             | o             | FILTER EG AMT         |
    |                            23 | x             | o             | EG DECAY              |
    +-------------------------------+---------------+---------------+-----------------------+
    | Program                       | x             | x             |                       |
    | Change       : True #         | ************* |               |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | System Exclusive              | x             | x             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | System       : Song Pos       | x             | x             |                       |
    | Common       : Song Sel       | x             | x             |                       |
    |              : Tune           | x             | x             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | System       : Clock          | x             | x             |                       |
    | Real Time    : Commands       | x             | x             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Aux          : Local ON/OFF   | x             | x             |                       |
    | Messages     : All Notes OFF  | x             | o             |                       |
    |              : Active Sense   | x             | x             |                       |
    |              : Reset          | x             | x             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Notes                         |                                                       |
    |                               |                                                       |
    +-------------------------------+-------------------------------------------------------+
      Mode 1: Omni On,  Poly          Mode 2: Omni On,  Mono          o: Yes                 
      Mode 3: Omni Off, Poly          Mode 4: Omni Off, Mono          x: No                  
