#pragma once

// refs http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt

#include "common.h"
#include "mul-q.h"
#include "filter-table.h"

template <uint8_t T>
class Filter {
  static uint8_t        m_count;
  static const uint8_t* m_lpf_table;
  static uint8_t        m_b_2_over_a_0_low;
  static int8_t         m_b_2_over_a_0_high;
  static int8_t         m_a_1_over_a_0_high;
  static int16_t        m_x_1;
  static int16_t        m_x_2;
  static int16_t        m_y_1;
  static int16_t        m_y_2;
  static uint8_t        m_cutoff;
  static uint8_t        m_cutoff_mod;

  static const uint8_t AUDIO_FRACTION_BITS = 14;

public:
  INLINE static void initialize() {
    m_count = 0;
    m_x_1 = 0;
    m_x_2 = 0;
    m_y_1 = 0;
    m_y_2 = 0;
    set_cutoff(127);
    set_resonance(0);
    set_env_amt(0);
  }

  INLINE static void set_cutoff(uint8_t controller_value) {
    m_cutoff = controller_value;
  }

  INLINE static void set_resonance(uint8_t controller_value) {
    m_lpf_table = g_filter_lpf_tables[controller_value >> 3];
  }

  INLINE static void set_env_amt(uint8_t controller_value) {
    m_cutoff_mod = controller_value;
  }

  INLINE static int16_t clock(int16_t audio_input, uint8_t cutoff_mod) {
    m_count++;
    if ((m_count & 0x03) == 0) {
      uint8_t cutoff = m_cutoff + high_byte((m_cutoff_mod + 1) * cutoff_mod);
      if (cutoff > 127) {
        cutoff = 127;
      }
      const uint8_t* p = m_lpf_table + (cutoff * 3);
      m_b_2_over_a_0_low  = pgm_read_byte(p++);
      m_b_2_over_a_0_high = pgm_read_byte(p++);
      m_a_1_over_a_0_high = pgm_read_byte(p);
    }

    int16_t b_2_over_a_0 = m_b_2_over_a_0_low | (m_b_2_over_a_0_high << 8);
    int16_t a_2_over_a_0 = (b_2_over_a_0 << 2) - (m_a_1_over_a_0_high << 8) -
                                                 (1 << FILTER_TABLE_FRACTION_BITS);

    int16_t x_0  = audio_input >> (16 - AUDIO_FRACTION_BITS);
    int16_t tmp  = mul_q15_q15(x_0 + (m_x_1 << 1) + m_x_2, b_2_over_a_0);
    tmp         -= mul_q15_q7( m_y_1,                      m_a_1_over_a_0_high);
    tmp         -= mul_q15_q15(m_y_2,                      a_2_over_a_0);
    int16_t y_0  = tmp << (16 - FILTER_TABLE_FRACTION_BITS);

    if (y_0 > ((1 << (AUDIO_FRACTION_BITS - 1)) - 1)) {
      y_0 = ((1 << (AUDIO_FRACTION_BITS - 1)) - 1);
    }
    if (y_0 < -(1 << (AUDIO_FRACTION_BITS - 1))) {
      y_0 = -(1 << (AUDIO_FRACTION_BITS - 1));
    }

    m_x_2 = m_x_1;
    m_y_2 = m_y_1;
    m_x_1 = x_0;
    m_y_1 = y_0;

    return y_0 << (16 - AUDIO_FRACTION_BITS);
  }
};

template <uint8_t T> uint8_t        Filter<T>::m_count;
template <uint8_t T> const uint8_t* Filter<T>::m_lpf_table;
template <uint8_t T> uint8_t        Filter<T>::m_b_2_over_a_0_low;
template <uint8_t T> int8_t         Filter<T>::m_b_2_over_a_0_high;
template <uint8_t T> int8_t         Filter<T>::m_a_1_over_a_0_high;
template <uint8_t T> int16_t        Filter<T>::m_x_1;
template <uint8_t T> int16_t        Filter<T>::m_x_2;
template <uint8_t T> int16_t        Filter<T>::m_y_1;
template <uint8_t T> int16_t        Filter<T>::m_y_2;
template <uint8_t T> uint8_t        Filter<T>::m_cutoff;
template <uint8_t T> uint8_t        Filter<T>::m_cutoff_mod;
