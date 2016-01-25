# Digital Synth VRA8-P 0.0.0

- 2016-00-00 ISGK Instruments
- <https://github.com/risgk/digital-synth-vra8-p>

## Concept

- Pseudo Polyphonic Synthesizer for Arduino Uno

## Features

- Pseudo Polyphonic Synthesizer (or MIDI Sound Module)
- Serial MIDI In (38400 bps), PWM Audio Out (Pin 6), PWM Rate: 62500 Hz
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
- CAUTION: Click sounds occur sometimes when you change the controllers

## Controllers

### Oscillator's

    +-------------+-----------+--------------+---------------+----------------+-------------+
    | OSC MODE    | Name      | Description  | OSC COLOR     | MOD RATE       | MOD DEPTH   |
    +-------------+-----------+--------------+---------------+----------------+-------------+
    | 0           | SAW       | Saw Wave     | PWM Mix       | Detune         | Multi Saws  |
    | (0 ~ 7)     |           |              | Off/On        | 0.24 ~ 15.3 Hz | Mix         |
    +-------------+-----------+--------------+---------------+----------------+-------------+
    | 16          | PULSE     | Pulse Wave   | Pulse Width   | Detune         | Detuned     |
    | (8 ~ 23)    |           |              | 50 ~ 0.4%     | 0.24 ~ 15.3 Hz | Pulse Mix   |
    +-------------+-----------+--------------+---------------+----------------+-------------+
    | 32          | PULSE+SAW | Pulse Wave + | Mix           | PWM Rate       | PWM Depth   |
    | (24 ~ 39)   |           | Saw Wave     | Pulse ~ Saw   | 0.24 ~ 15.3 Hz |             |
    +-------------+-----------+--------------+---------------+----------------+-------------+
    | 48          | ----      | Reserved     | ----          | ----           | ----        |
    | (40 ~ 55)   |           |              |               |                |             |
    +-------------+-----------+--------------+---------------+----------------+-------------+
    | 64          | ORGAN     | Organ Like   | Harmonics     | Vib. Rate      | Vib. Depth  |
    | (56 ~ 71)   |           |              | Control       | 0.24 ~ 15.3 Hz |             |
    +-------------+-----------+--------------+---------------+----------------+-------------+
    | 80          | ----      | Reserved     | ----          | ----           | ----        |
    | (72 ~ 87)   |           |              |               |                |             |
    +-------------+-----------+--------------+---------------+----------------+-------------+
    | 96          | BINARY    | Binary Wave  | Bits Off/On   | PWM Rate       | PWM Depth   |
    | (88 ~ 103)  |           | (?)          | Control       | 0.24 ~ 15.3 Hz |             |
    +-------------+-----------+--------------+---------------+----------------+-------------+
    | 112         | ----      | Reserved     | ----          | ----           | ----        |
    | (104 ~ 119) |           |              |               |                |             |
    +-------------+-----------+--------------+---------------+----------------+-------------+
    | 127         | FM        | FM Synth     | Detune        | Freq. Ratio    | Mod. Index  |
    | (120 ~ 127) |           | (PM Synth)   | 0 ~ 15.0 Hz   | 0.5 ~ 16.0     | 0 ~ 12.6    |
    +-------------+-----------+--------------+---------------+----------------+-------------+

### Others'

    +----------------+-----------------+-----------------+-----------------+----------------+
    | Controller     | Value 0         | Value 64        | Value 127       | Notes          |
    +----------------+-----------------+-----------------+-----------------+----------------+
    | LPF CUTOFF/ENV | 7.0 ~ 7.0 kHz   | 0.2 ~ 0.2 kHz   | 0.2 ~ 7.0 kHz   | Env 0 ~ 100 %  |
    +----------------+-----------------+-----------------+-----------------+----------------+
    | LPF RESONANCE  | Q = 0.7         | Q = 2.8         | Q = 8.0         | 3 Steps        |
    +----------------+-----------------+-----------------+-----------------+----------------+
    | ENV A          | 4.2 ms          | 0.14 s          | 4.2 s           |                |
    +----------------+-----------------+-----------------+-----------------+----------------+
    | ENV D/R        | Decay 10.0 ms   | Decay Infinite  | Decay Infinite  |                |
    |                | Release 10.0 ms | Release 10.0 s  | Release 10.0 ms |                |
    +----------------+-----------------+-----------------+-----------------+----------------+

## MIDI Implementation Chart

      [Experimental Synthesizer]                                      Date: 2016-00-00       
      Model  Digital Synth VRA8-P     MIDI Implementation Chart       Version: 0.0.0         
    +-------------------------------+---------------+---------------+-----------------------+
    | Function...                   | Transmitted   | Recognized    | Remarks               |
    +-------------------------------+---------------+---------------+-----------------------+
    | Basic        Default          | x             | 1             |                       |
    | Channel      Changed          | x             | x             |                       |
    +-------------------------------+---------------+---------------+-----------------------+
    | Mode         Default          | x             | Mode 4 (M=1)  |                       |
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
    | Control                    16 | x             | o             | OSC MODE              |
    | Change                     17 | x             | o             | OSC COLOR             |
    |                            18 | x             | o             | MOD RATE              |
    |                            19 | x             | o             | MOD DEPTH             |
    |                            20 | x             | o             | LPF CUTOFF/ENV        |
    |                            21 | x             | o             | LPF RESONANCE         |
    |                            22 | x             | o             | ENV A                 |
    |                            23 | x             | o             | ENV D/R               |
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
