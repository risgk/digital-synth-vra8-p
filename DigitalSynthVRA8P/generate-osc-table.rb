require_relative 'common'

$file = File.open("osc-table.h", "w")

$file.printf("#pragma once\n\n")

def freq_from_note_number(note_number)
  cent = (note_number * 100.0) - 6900.0
  hz = A4_PITCH * (2.0 ** (cent / 1200.0))
  freq = (hz * (1 << OSC_PHASE_RESOLUTION_BITS) / SAMPLING_RATE).floor.to_i
  freq = freq + 1 if freq.even?
  freq
end

$file.printf("const __uint24 g_osc_freq_table[] = {\n  ")
(NOTE_NUMBER_MIN..NOTE_NUMBER_MAX).each do |note_number|
  if (note_number < NOTE_NUMBER_MIN) || (note_number > NOTE_NUMBER_MAX)
    freq = 0
  else
    freq = freq_from_note_number(note_number)
  end

  $file.printf("0x%06X,", freq)
  if note_number == DATA_BYTE_MAX
    $file.printf("\n")
  elsif note_number % 6 == 5
    $file.printf("\n  ")
  else
    $file.printf(" ")
  end
end
$file.printf("};\n\n")

def generate_osc_wave_table_sawtooth(last)
  $file.printf("const uint8_t g_osc_saw_wave_table_h%d[] PROGMEM = {\n  ", last)
  (0..(1 << OSC_WAVE_TABLE_SAMPLES_BITS)).each do |n|
    level = 0
    (1..last).each do |k|
      level += (2.0 / Math::PI) * Math.sin((2.0 * Math::PI) * ((n + 0.5) /
               (1 << OSC_WAVE_TABLE_SAMPLES_BITS)) * k) / k
    end
    level = (level * OSC_WAVE_TABLE_AMPLITUDE).floor.to_i

    level += 0x100 if level < 0
    $file.printf("0x%02X,", level)
    if n == (1 << OSC_WAVE_TABLE_SAMPLES_BITS)
      $file.printf("\n")
    elsif n % 16 == 15
      $file.printf("\n  ")
    else
      $file.printf(" ")
    end
  end
  $file.printf("};\n\n")
end

def generate_osc_wave_table_sine(last)
  $file.printf("const uint8_t g_osc_sine_wave_table_h%d[] PROGMEM = {\n  ", last)
  (0..(1 << OSC_WAVE_TABLE_SAMPLES_BITS)).each do |n|
    level = 0
    level += Math::sin((2.0 * Math::PI) * ((n + 0.5) / (1 << OSC_WAVE_TABLE_SAMPLES_BITS)))
    level = (level * OSC_WAVE_TABLE_AMPLITUDE).floor.to_i

    level += 0x100 if level < 0
    $file.printf("0x%02X,", level)
    if n == (1 << OSC_WAVE_TABLE_SAMPLES_BITS)
      $file.printf("\n")
    elsif n % 16 == 15
      $file.printf("\n  ")
    else
      $file.printf(" ")
    end
  end
  $file.printf("};\n\n")
end

$osc_harmonics_restriction_table = []

(NOTE_NUMBER_MIN..NOTE_NUMBER_MAX).each do |note_number|
  freq = freq_from_note_number(note_number)
  $osc_harmonics_restriction_table << freq
end

def last_harmonic(freq)
  last = (freq != 0) ? ((FREQUENCY_MAX *
                         (1 << OSC_PHASE_RESOLUTION_BITS)) / (freq * SAMPLING_RATE)) : 0
  last = 127 if last > 127
  last
end

$osc_harmonics_restriction_table.map { |freq| last_harmonic(freq) }.uniq.sort.reverse.each do |i|
  generate_osc_wave_table_sawtooth(i)
end

$file.printf("const uint8_t* g_osc_saw_wave_tables[] = {\n  ")
$osc_harmonics_restriction_table.each_with_index do |freq, idx|
  $file.printf("g_osc_saw_wave_table_h%-3d,", last_harmonic(freq))
  if idx == DATA_BYTE_MAX
    $file.printf("\n")
  elsif idx % 4 == 3
    $file.printf("\n  ")
  else
    $file.printf(" ")
  end
end
$file.printf("};\n\n")

generate_osc_wave_table_sine(1)

$file.close
