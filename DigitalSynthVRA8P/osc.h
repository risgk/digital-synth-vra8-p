#pragma once

#include "common.h"
#include "mul-q.h"
#include "osc-table.h"

template <uint8_t T>
class Osc {
  static boolean        m_mono;
  static uint16_t       m_phase[6];
  static uint16_t       m_freq[4];
  static const uint8_t* m_wave_table[4];
  static uint8_t        m_color;
  static uint16_t       m_detune;
  static uint8_t        m_param_binary;

public:
  INLINE static void initialize() {
    m_freq[0] = g_osc_freq_table[NOTE_NUMBER_MIN];
    m_freq[1] = g_osc_freq_table[NOTE_NUMBER_MIN];
    m_freq[2] = g_osc_freq_table[NOTE_NUMBER_MIN];
    m_freq[3] = g_osc_freq_table[NOTE_NUMBER_MIN];
    m_wave_table[0] = g_osc_saw_wave_tables[0];
    m_wave_table[1] = g_osc_saw_wave_tables[0];
    m_wave_table[2] = g_osc_saw_wave_tables[0];
    m_wave_table[3] = g_osc_saw_wave_tables[0];
    reset_phase();
    set_saw_sq(0);
    set_detune(0);
    m_param_binary = m_color;
  }

  INLINE static void set_poly_mono(uint8_t controller_value) {
    if (controller_value >= 64) {
      m_mono = true;
    } else {
      m_mono = false;
    }
  }

  INLINE static void set_saw_sq(uint8_t controller_value) {
    m_color = controller_value;
  }

  INLINE static void set_detune(uint8_t controller_value) {
    m_detune = (controller_value >> 2) + 1;
    if (m_mono) {
      m_freq[1] = m_freq[0] + (m_detune << 2);
      m_freq[2] = m_freq[0] - (m_detune << 2);
    }
  }

  INLINE static void note_on(uint8_t osc_number, uint8_t note_number) {
    if (m_mono) {
      m_wave_table[0] = g_osc_saw_wave_tables[note_number - NOTE_NUMBER_MIN];
      m_wave_table[1] = m_wave_table[0];
      m_wave_table[2] = m_wave_table[0];
      m_freq[0] = g_osc_freq_table[note_number - NOTE_NUMBER_MIN];
      m_freq[1] = m_freq[0] + (m_detune << 1);
      m_freq[2] = m_freq[0] - (m_detune << 1);
    } else {
      m_wave_table[osc_number] = g_osc_saw_wave_tables[note_number - NOTE_NUMBER_MIN];
      m_freq[osc_number] = g_osc_freq_table[note_number - NOTE_NUMBER_MIN];
    }
  }

  INLINE static void note_off(uint8_t osc_number) {
    // do nothing
  }

  INLINE static int16_t clock(uint8_t eg_control_0,
                              uint8_t eg_control_1,
                              uint8_t eg_control_2) {
    int16_t result = 0;

    m_phase[0] += m_freq[0];
    m_phase[1] += m_freq[1];
    m_phase[2] += m_freq[2];
    m_phase[3] -= m_mono ? m_detune : (m_detune << 1);

    int8_t wave_0_0 = get_wave_level(m_wave_table[0], m_phase[0]);
    int8_t wave_1_0 = get_wave_level(m_wave_table[1], m_phase[1]);
    int8_t wave_2_0 = get_wave_level(m_wave_table[2], m_phase[2]);
    int8_t wave_0_1 = get_wave_level(m_wave_table[0], m_phase[0] + m_phase[3]);
    int8_t wave_1_1 = get_wave_level(m_wave_table[1], m_phase[1] + m_phase[3]);
    int8_t wave_2_1 = get_wave_level(m_wave_table[2], m_phase[2] + m_phase[3]);

    // amps and a mixer
    result = ((wave_0_0 + wave_0_1) * (eg_control_0 >> 2)) +
             ((wave_1_0 + wave_1_1) * (eg_control_1 >> 2)) +
             ((wave_2_0 + wave_2_1) * (eg_control_2 >> 2));
    result >>= 1;

    return result;
  }

private:
  INLINE static void reset_phase() {
    m_phase[0] = 127;
    m_phase[1] = 127;
    m_phase[2] = 127;
    m_phase[3] = 127;
    m_freq[0] = 0;
    m_freq[1] = 0;
    m_freq[2] = 0;
    m_freq[3] = 0;
  }

  INLINE static uint8_t value_to_low_freq(uint8_t value) {
    return value >> 1;
  }

  INLINE static uint8_t mod_rate_to_fm_ratio(uint8_t mod_rate) {
    return (mod_rate >> 2) + 1;
  }

  INLINE static int8_t triangle_lfo_clock(uint8_t mod_eg_control, uint8_t mod_rate) {
    m_phase[5] += value_to_low_freq(mod_rate) + 1;
    uint16_t level = m_phase[5];
    if ((level & 0x8000) != 0) {
      level = ~level;
    }
    level -= 0x4000;
    return high_sbyte((high_sbyte(level << 1) + 1) * mod_eg_control);
  }

  INLINE static int8_t get_triangle_wave_level(uint16_t phase) {
    uint8_t level = high_byte(phase) + 0x40;
    if ((level & 0x80) != 0) {
      level = ~level;
    }
    level -= 0x40;
    return static_cast<int8_t>(level);
  }

  INLINE static int8_t get_wave_level(const uint8_t* wave_table, uint16_t phase) {
    uint8_t curr_index = high_byte(phase);
    uint16_t tmp = pgm_read_word(wave_table + curr_index);
    int8_t curr_data = low_byte(tmp);
    int8_t next_data = high_byte(tmp);
    uint8_t next_weight = low_byte(phase);

    // lerp
    int8_t level = curr_data +
                   high_sbyte(static_cast<int8_t>(next_data - curr_data) * next_weight);

    return level;
  }

  INLINE static uint8_t nth_bit(uint8_t byte, uint8_t n) {
    return (byte >> n) & 1;
  }
};

template <uint8_t T> boolean         Osc<T>::m_mono;
template <uint8_t T> uint16_t        Osc<T>::m_phase[6];
template <uint8_t T> uint16_t        Osc<T>::m_freq[4];
template <uint8_t T> const uint8_t*  Osc<T>::m_wave_table[4];
template <uint8_t T> uint8_t         Osc<T>::m_color;
template <uint8_t T> uint16_t        Osc<T>::m_detune;
template <uint8_t T> uint8_t         Osc<T>::m_param_binary;
