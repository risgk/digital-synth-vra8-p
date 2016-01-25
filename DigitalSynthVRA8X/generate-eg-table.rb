require_relative 'common'

$file = File.open("eg-table.h", "w")

$file.printf("#pragma once\n\n")

EG_ATTACK_UPDATE_INTERVAL = 4
$file.printf("const uint8_t EG_ATTACK_UPDATE_INTERVAL = %d;\n", EG_ATTACK_UPDATE_INTERVAL)
$file.printf("\n")

$file.printf("const uint16_t g_eg_attack_rate_table[] = {\n  ")
(0..((1 << EG_CONTROLLER_STEPS_BITS) - 1)).each do |time|
  t = time
  t = 1 if t == 0
  sec = 4.161536 / (10.0 ** ((((1 << EG_CONTROLLER_STEPS_BITS) - 1) - t) /
                             (((1 << EG_CONTROLLER_STEPS_BITS) - 2) / 3.0)))
  rate = (EG_LEVEL_MAX * EG_ATTACK_UPDATE_INTERVAL / (sec * SAMPLING_RATE)).floor.to_i

  $file.printf("0x%04X,", rate)
  if time == DATA_BYTE_MAX
    $file.printf("\n")
  elsif time % 16 == 15
    $file.printf("\n  ")
  else
    $file.printf(" ")
  end
end
$file.printf("};\n\n")

def decay_release_rate(t)
  u = [[(t - 2) / 5, 0].max, 7].min
  rate = (((1.0 / 2.0) ** (1.0 / (2.0 ** u)))  *
          (1 << EG_DECAY_RELEASE_RATE_DENOMINATOR_BITS)).floor
end

def decay_release_update_interval(t)
  sec = 10.0 / (10.0 ** ((((1 << EG_CONTROLLER_STEPS_BITS) - 1) - t) /
                         (((1 << EG_CONTROLLER_STEPS_BITS) - 2) / 3.0)))
  sec = sec * 1.125  # correction

  update_interval = ((sec * SAMPLING_RATE) /
                     (Math.log(1.0 / 256.0) /
                      Math.log(decay_release_rate(t).to_f /
                      (1 << EG_DECAY_RELEASE_RATE_DENOMINATOR_BITS)))
                    ).floor.to_i
end

$file.printf("const uint8_t g_eg_decay_release_rate_table[] = {\n  ")
(0..((1 << EG_CONTROLLER_STEPS_BITS) - 1)).each do |time|
  t = time
  t = 1 if t == 0
  rate = decay_release_rate(t)

  $file.printf("0x%02X,", rate)
  if time == DATA_BYTE_MAX
    $file.printf("\n")
  elsif time % 16 == 15
    $file.printf("\n  ")
  else
    $file.printf(" ")
  end
end
$file.printf("};\n\n")

$file.printf("const uint16_t g_eg_decay_release_update_interval_table[] = {\n  ")
(0..((1 << EG_CONTROLLER_STEPS_BITS) - 1)).each do |time|
  t = time
  t = 1 if t == 0
  update_interval = decay_release_update_interval(t)

  $file.printf("0x%04X,", update_interval)
  if time == DATA_BYTE_MAX
    $file.printf("\n")
  elsif time % 16 == 15
    $file.printf("\n  ")
  else
    $file.printf(" ")
  end
end
$file.printf("};\n\n")

$file.close
