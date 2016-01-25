#pragma once

#include "common.h"
#include "mul-q.h"
#include "osc-table.h"

template <uint8_t T>
class Osc {
  static const uint8_t* m_wave_table;
  static uint16_t       m_freq;
  static uint16_t       m_phase[6];
  static uint8_t        m_mode;
  static uint8_t        m_color;
  static uint16_t       m_mod_rate;
  static uint16_t       m_mod_depth;
  static uint8_t        m_param_binary;

public:
  INLINE static void initialize() {
    m_wave_table = g_osc_sine_wave_table_h1;
    m_freq = g_osc_freq_table[NOTE_NUMBER_MIN];
    reset_phase();
    set_mode(0);
    set_color(0);
    set_mod_rate(0);
    set_mod_depth(0);
    m_param_binary = m_color;
  }

  INLINE static void set_mode(uint8_t controller_value) {
    uint8_t mode = (static_cast<uint8_t>(controller_value + 8) >> 4) << 4;
    if (mode >= 128) {
      mode = 127;
    }
    if (m_mode != mode) {
      m_mode = mode;
      reset_phase();
    }
  }

  INLINE static void set_color(uint8_t controller_value) {
    switch (m_mode) {
    case OSC_MODE_FM:
      {
        uint8_t old_low_freq = (value_to_low_freq(m_color) >> 1) << 1;
        uint8_t new_low_freq = (value_to_low_freq(controller_value) >> 1) << 1;
        if (old_low_freq != 0 && new_low_freq == 0) {
          reset_phase();
        }
      }
      break;
    }

    m_color = controller_value;
  }

  INLINE static void set_mod_rate(uint8_t controller_value) {
    switch (m_mode) {
    case OSC_MODE_FM:
      {
        uint8_t old_fm_ratio = mod_rate_to_fm_ratio(m_mod_rate);
        uint8_t new_fm_ratio = mod_rate_to_fm_ratio(controller_value);
        if (old_fm_ratio != new_fm_ratio) {
          reset_phase();
        }
      }
      break;
    }

    m_mod_rate = controller_value;
  }

  INLINE static void set_mod_depth(uint8_t controller_value) {
    m_mod_depth = controller_value << 1;
  }

  INLINE static void note_on(uint8_t note_number) {
    m_wave_table = g_osc_saw_wave_tables[note_number - NOTE_NUMBER_MIN];
    m_freq = g_osc_freq_table[note_number - NOTE_NUMBER_MIN];
  }

  INLINE static int16_t clock(uint8_t mod_eg_control) {
    int16_t result = 0;

    switch (m_mode) {
    case OSC_MODE_FM:
      {
        uint8_t low_freq = (value_to_low_freq(m_color) >> 1) << 1;
        uint16_t freq_detune = m_freq - low_freq;

        uint8_t fm_ratio = mod_rate_to_fm_ratio(m_mod_rate);
        uint16_t mod_freq = (m_freq >> 1) * fm_ratio;
        uint16_t mod_freq_detune = (freq_detune >> 1) * fm_ratio;

        m_phase[2] += mod_freq;
        m_phase[3] += mod_freq_detune;

        m_phase[0] += m_freq;
        m_phase[1] += freq_detune;

        int8_t wave_2 = get_wave_level(g_osc_sine_wave_table_h1, m_phase[2]);
        int8_t wave_3 = get_wave_level(g_osc_sine_wave_table_h1, m_phase[3]);

        int8_t wave_0 = get_wave_level(g_osc_sine_wave_table_h1,
                                       m_phase[0] + ((wave_2 * m_mod_depth) << 3));
        int8_t wave_1 = get_wave_level(g_osc_sine_wave_table_h1,
                                       m_phase[1] + ((wave_3 * m_mod_depth) << 3));

        result = (wave_0 * 64) + (wave_1 * 32);
      }
      break;
    case OSC_MODE_BINARY:
      {
        int8_t mod_lfo_control = triangle_lfo_clock(mod_eg_control, m_mod_rate);
        int16_t freq = m_freq + high_sbyte((m_mod_depth + 1) * mod_lfo_control);

        uint16_t phase_old = m_phase[0];
        m_phase[0] += freq;
        if (m_phase[0] < phase_old) {
          m_param_binary = (m_color == 127) ? ((126 << 1) | 1) : ((m_color << 1) | 1);
        }

        uint8_t curr_index = high_byte(m_phase[0]) >> 4;
        uint8_t next_index = static_cast<uint8_t>(high_byte(m_phase[0]) + 16) >> 4;
        uint16_t curr_p = (nth_bit(m_param_binary, curr_index >> 1) != 0) ? 0x4000 : 0xC000;
        uint16_t next_p = (nth_bit(m_param_binary, next_index >> 1) != 0) ? 0x4000 : 0xC000;
        uint16_t next_w = (m_phase[0] & 0xFFF) << 3;

        if (curr_p == next_p) {
          result = get_wave_level(g_osc_sine_wave_table_h1, curr_p) * 95;
        } else {
          result = get_wave_level(g_osc_sine_wave_table_h1, curr_p + next_w) * 95;
        }
      }
      break;
    case OSC_MODE_PULSE_SAW:
      {
        int8_t mod_lfo_control = triangle_lfo_clock(mod_eg_control, m_mod_rate);
        int16_t shift_lfo = mod_lfo_control * m_mod_depth;

        m_phase[0] += m_freq;

        int8_t saw_down      = get_wave_level(m_wave_table, m_phase[0]);
        int8_t saw_up        = get_wave_level(m_wave_table, m_phase[0] + (128 << 8) - shift_lfo);
        int8_t saw_down_copy = get_wave_level(m_wave_table, m_phase[0]              + shift_lfo);

        int16_t mixed = +(saw_down      * 127) +
                        -(saw_up        * static_cast<uint8_t>(127 - m_color)) +
                        +(saw_down_copy * m_color);
        result = mixed >> 1;
      }
      break;
    case OSC_MODE_PULSE:
      {
        uint8_t low_freq = value_to_low_freq(m_mod_rate) + 1;
        uint16_t freq_detune = m_freq - low_freq;
        m_phase[0] += m_freq;
        m_phase[1] += freq_detune;

        uint16_t pulse_width = (m_color + 128) << 8;
        int8_t saw_down_0 = get_wave_level(m_wave_table, m_phase[0]);
        int8_t saw_up_0   = get_wave_level(m_wave_table, m_phase[0] + pulse_width);
        int8_t saw_down_1 = get_wave_level(m_wave_table, m_phase[1]);
        int8_t saw_up_1   = get_wave_level(m_wave_table, m_phase[1] + pulse_width);

        uint8_t m = (m_mod_depth * 3) >> 3;
        int16_t mixed = ((saw_down_0 - saw_up_0) * static_cast<uint8_t>(190 - m)) +
                        ((saw_down_1 - saw_up_1) * m);
        result = mixed >> 1;
      }
      break;
    case OSC_MODE_ORGAN:
      {
        int8_t mod_lfo_control = triangle_lfo_clock(mod_eg_control, m_mod_rate);
        int16_t freq = m_freq + high_sbyte((m_mod_depth + 1) * mod_lfo_control);

        m_phase[0] += (freq >> 1);
        m_phase[1] += (freq >> 1) + freq;
        m_phase[2] += (freq << 2) + freq;
        result = get_organ_level(m_color, m_phase[0], m_phase[1], m_phase[2]);
      }
      break;
    case OSC_MODE_SAW:
      {
        uint16_t low_freq = value_to_low_freq(m_mod_rate) + 1;
        uint16_t low_freq_x2 = low_freq << 1;
        m_phase[0] += m_freq;
        m_phase[1] += m_freq - low_freq;
        m_phase[2] += m_freq + low_freq;
        m_phase[3] += m_freq - low_freq_x2;
        m_phase[5] += m_freq - low_freq_x2 - low_freq;
        m_phase[4] += m_freq + low_freq_x2;

        int8_t wave_0 = get_wave_level(m_wave_table, m_phase[0]);
        int8_t wave_1 = get_wave_level(m_wave_table, m_phase[1]);
        int8_t wave_2 = get_wave_level(m_wave_table, m_phase[2]);
        int8_t wave_3 = get_wave_level(m_wave_table, m_phase[3]);
        int8_t wave_4 = get_wave_level(m_wave_table, m_phase[4]);
        int8_t wave_5 = get_wave_level(m_wave_table, m_phase[5]);

        uint8_t d = (m_mod_depth * 5) >> 6;
        int8_t r = (m_color < 64) ? d : -d;
        result = (wave_0 * (117 - (d << 1))) + ((wave_1 + wave_2) * r) +
                 ((wave_3 + wave_4) * d) + (wave_5 * r);
      }
      break;
    default:
      {
        m_phase[0] += m_freq;
        result = get_wave_level(g_osc_sine_wave_table_h1, m_phase[0]) * 127;
      }
      break;
    }

    return result;
  }

private:
  INLINE static void reset_phase() {
    m_phase[0] = 0;
    m_phase[1] = 0;
    m_phase[2] = 0;
    m_phase[3] = 0;
    m_phase[4] = 0;
    m_phase[5] = 0;
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


  INLINE static int16_t get_organ_level(uint8_t color, uint16_t phase_0,
                                                       uint16_t phase_1_5, uint16_t phase_5) {
    uint8_t c = 0;
    int16_t result = 0;
    result += (color & (1 << 0)) ? (c++, get_triangle_wave_level(phase_0   << 0)) : 0;  // 0.5
    result += (color & (1 << 0)) ? (c++, get_triangle_wave_level(phase_1_5 << 0)) : 0;  // 1.5
    result +=                      (c++, get_triangle_wave_level(phase_0   << 1))    ;  // 1
    result += (color & (1 << 1)) ? (c++, get_triangle_wave_level(phase_0   << 2)) : 0;  // 2
    result += (color & (1 << 2)) ? (c++, get_triangle_wave_level(phase_1_5 << 1)) : 0;  // 3
    result += (color & (1 << 3)) ? (c++, get_triangle_wave_level(phase_0   << 3)) : 0;  // 4
    result += (color & (1 << 4)) ? (c++, get_triangle_wave_level(phase_5   << 0)) : 0;  // 5
    result += (color & (1 << 5)) ? (c++, get_triangle_wave_level(phase_1_5 << 2)) : 0;  // 6
    result += (color & (1 << 6)) ? (c++, get_triangle_wave_level(phase_0   << 4)) : 0;  // 8

    switch (c) {
    case 1: result *= 96; break;
    case 2: result *= 68; break;
    case 3: result *= 55; break;
    case 4: result *= 48; break;
    case 5: result *= 43; break;
    case 6: result *= 39; break;
    case 7: result *= 36; break;
    case 8: result *= 34; break;
    case 9: result *= 32; break;
    }

    return result;
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

template <uint8_t T> const uint8_t*  Osc<T>::m_wave_table;
template <uint8_t T> uint16_t        Osc<T>::m_freq;
template <uint8_t T> uint16_t        Osc<T>::m_phase[6];
template <uint8_t T> uint8_t         Osc<T>::m_mode;
template <uint8_t T> uint8_t         Osc<T>::m_color;
template <uint8_t T> uint16_t        Osc<T>::m_mod_rate;
template <uint8_t T> uint16_t        Osc<T>::m_mod_depth;
template <uint8_t T> uint8_t         Osc<T>::m_param_binary;
