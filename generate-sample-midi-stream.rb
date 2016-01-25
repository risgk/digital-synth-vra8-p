require_relative 'DigitalSynthVRA8P/common'

$file = File.open("sample-midi-stream.bin", "wb")

def control_change(control_number, value)
  $file.write([(CONTROL_CHANGE | MIDI_CH), control_number, value].pack("C*"))
end

def play(note_number, length)
  $file.write([(NOTE_ON  | MIDI_CH), note_number, 64].pack("C*"))
  (length * 7 / 8).times { $file.write([ACTIVE_SENSING].pack("C")) }
  $file.write([(NOTE_OFF | MIDI_CH), note_number, 64].pack("C*"))
  (length * 1 / 8).times { $file.write([ACTIVE_SENSING].pack("C")) }
end

def wait(length)
  length.times { $file.write([ACTIVE_SENSING].pack("C")) }
end

def play_cegbdfac(c)
  play(12 + (c * 12), 1200)
  play(16 + (c * 12), 1200)
  play(19 + (c * 12), 1200)
  play(23 + (c * 12), 1200)
  play(14 + (c * 12), 1200)
  play(17 + (c * 12), 1200)
  play(21 + (c * 12), 1200)
  play(24 + (c * 12), 6400)
  wait(6400)
end

def sound_off
  control_change(ENV_D_R      , 0  )
  control_change(ALL_NOTES_OFF, 0  )
  wait(800)
end

sound_off
control_change(OSC_MODE      , 0  )
control_change(OSC_COLOR     , 127)
control_change(MOD_RATE      , 8  )
control_change(MOD_DEPTH     , 127)
control_change(LPF_CUTOFF_ENV, 127)
control_change(LPF_RESONANCE , 127)
control_change(ENV_A         , 64 )
control_change(ENV_D_R       , 86 )
play_cegbdfac(3)

sound_off
control_change(OSC_MODE      , 16 )
control_change(OSC_COLOR     , 0  )
control_change(MOD_RATE      , 8  )
control_change(MOD_DEPTH     , 64 )
control_change(LPF_CUTOFF_ENV, 0  )
control_change(LPF_RESONANCE , 0  )
control_change(ENV_A         , 44 )
control_change(ENV_D_R       , 96 )
play_cegbdfac(4)

sound_off
control_change(OSC_MODE      , 32 )
control_change(OSC_COLOR     , 64 )
control_change(MOD_RATE      , 8  )
control_change(MOD_DEPTH     , 32 )
control_change(LPF_CUTOFF_ENV, 0  )
control_change(LPF_RESONANCE , 64 )
control_change(ENV_A         , 44 )
control_change(ENV_D_R       , 96 )
play_cegbdfac(3)

sound_off
control_change(OSC_MODE      , 64 )
control_change(OSC_COLOR     , 127)
control_change(MOD_RATE      , 64 )
control_change(MOD_DEPTH     , 8  )
control_change(LPF_CUTOFF_ENV, 0  )
control_change(LPF_RESONANCE , 0  )
control_change(ENV_A         , 4  )
control_change(ENV_D_R       , 96 )
play_cegbdfac(4)

sound_off
control_change(OSC_MODE      , 96 )
control_change(OSC_COLOR     , 35 )
control_change(MOD_RATE      , 64 )
control_change(MOD_DEPTH     , 8  )
control_change(LPF_CUTOFF_ENV, 127)
control_change(LPF_RESONANCE , 0  )
control_change(ENV_A         , 44 )
control_change(ENV_D_R       , 96 )
play_cegbdfac(4)

sound_off
control_change(OSC_MODE      , 127)
control_change(OSC_COLOR     , 8  )
control_change(MOD_RATE      , 24 )
control_change(MOD_DEPTH     , 32 )
control_change(LPF_CUTOFF_ENV, 127)
control_change(LPF_RESONANCE , 0  )
control_change(ENV_A         , 4  )
control_change(ENV_D_R       , 52 )
play_cegbdfac(4)

sound_off

$file.close
