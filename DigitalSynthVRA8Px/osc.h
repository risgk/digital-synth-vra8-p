#pragma once

#include "common.h"
#include "osc-table.h"
#include "mul-q.h"
#include <math.h>

static const uint8_t OSC_MIX_TABLE_LENGTH = 64 + 1;  // odd number

template <uint8_t T>
class Osc {
  static boolean        m_unison_on;
  static uint8_t        m_mix_main;
  static uint8_t        m_mix_detune;
  static uint8_t        m_mix_table[OSC_MIX_TABLE_LENGTH];
  static uint8_t        m_waveform;
  static uint8_t        m_sub;
  static const uint8_t* m_wave_table[3];
  static __uint24       m_freq_array[3];
  static uint16_t       m_freq_detune;
  static __uint24       m_phase_array[3];
  static __uint24       m_phase_detune;

public:
  INLINE static void initialize() {
    m_unison_on = false;
    m_mix_main   = m_mix_table[(OSC_MIX_TABLE_LENGTH - 1) >> 1];
    m_mix_detune = m_mix_table[(OSC_MIX_TABLE_LENGTH - 1) >> 1];
    for (uint8_t i = 0; i < OSC_MIX_TABLE_LENGTH; i++) {
      m_mix_table[i] = (uint8_t) (sqrtf((float) i / (OSC_MIX_TABLE_LENGTH - 1)) * 255);
    }
    m_waveform = OSC_WAVEFORM_SAW;
    m_sub = 0;
    m_wave_table[0] = g_osc_saw_wave_tables[0];
    m_wave_table[1] = g_osc_saw_wave_tables[2];
    m_wave_table[2] = g_osc_saw_wave_tables[4];
    m_freq_array[0] = g_osc_freq_table[0];
    m_freq_array[1] = g_osc_freq_table[2];
    m_freq_array[2] = g_osc_freq_table[4];
    m_freq_detune = 0;
    m_phase_array[0] = 0;
    m_phase_array[1] = 0;
    m_phase_array[2] = 0;
    m_phase_detune = 0;
  }

  INLINE static void set_unison(boolean unison_on) {
    m_unison_on = unison_on;
  }

  INLINE static void set_mix(uint8_t mix) {
    if (mix < (OSC_MIX_TABLE_LENGTH - 1) >> 1) {
      m_mix_main   = m_mix_table[((OSC_MIX_TABLE_LENGTH - 1) >> 1) + mix];
      m_mix_detune = m_mix_table[((OSC_MIX_TABLE_LENGTH - 1) >> 1) - mix];
    } else {
      m_mix_main   = m_mix_table[OSC_MIX_TABLE_LENGTH - 1];
      m_mix_detune = m_mix_table[0];
    }
  }

  INLINE static void set_waveform(uint8_t waveform) {
    m_waveform = waveform;
  }

  INLINE static void set_sub(uint8_t controller_value) {
    m_sub = controller_value << 1;
  }

  INLINE static void set_detune(uint8_t controller_value) {
    m_freq_detune = ((uint16_t) high_byte((controller_value << 1) *
                                          (controller_value << 1)) <<
                     OSC_DETUNE_MUL_NUM_BITS) + OSC_DETUNE_FREQ_MIN;

    if (m_unison_on) {
      m_freq_array[1] = m_freq_array[0] + (m_freq_detune << 1);
      m_freq_array[2] = m_freq_array[0] - (m_freq_detune << 1);
    }
  }

  INLINE static void note_on(uint8_t osc_number, uint8_t note_number) {
    if (m_unison_on) {
      m_wave_table[0] = get_wave_table(m_waveform, note_number);
      m_wave_table[1] = m_wave_table[0];
      m_wave_table[2] = m_wave_table[0];
      m_freq_array[0] = g_osc_freq_table[note_number - NOTE_NUMBER_MIN];
      m_freq_array[1] = m_freq_array[0] + (m_freq_detune << 1);
      m_freq_array[2] = m_freq_array[0] - (m_freq_detune << 1);
    } else {
      m_wave_table[osc_number] = get_wave_table(m_waveform, note_number);
      m_freq_array[osc_number] = g_osc_freq_table[note_number - NOTE_NUMBER_MIN];
    }
  }

  INLINE static int16_t clock(uint8_t amp_0, uint8_t amp_1, uint8_t amp_2) {
    m_phase_array[0] += m_freq_array[0];
    m_phase_array[1] += m_freq_array[1];
    m_phase_array[2] += m_freq_array[2];
    m_phase_detune += m_freq_detune;

    int16_t result;
    if (m_waveform == OSC_WAVEFORM_ORGAN) {
      int8_t wave_0_main   = get_wave_level(m_wave_table[0], m_phase_array[0]);
      int8_t wave_1_main   = get_wave_level(m_wave_table[1], m_phase_array[1]);
      int8_t wave_2_main   = get_wave_level(m_wave_table[2], m_phase_array[2]);
      int8_t wave_0_detune = get_wave_level(m_wave_table[0], m_phase_array[0] + m_phase_detune);
      int8_t wave_1_detune = get_wave_level(m_wave_table[1], m_phase_array[1] + m_phase_detune);
      int8_t wave_2_detune = get_wave_level(m_wave_table[2], m_phase_array[2] + m_phase_detune);

      // amp and mix
      int16_t level_main   = mul_q15_q8((wave_0_main   * amp_0) +
                                        (wave_1_main   * amp_1) +
                                        (wave_2_main   * amp_2), m_mix_main);
      int16_t level_detune = mul_q15_q8((wave_0_detune * amp_0) +
                                        (wave_1_detune * amp_1) +
                                        (wave_2_detune * amp_2), m_mix_detune);
      result = level_main + level_detune;
    } else {
      int8_t wave_0_main   = get_wave_level(m_wave_table[0], (m_phase_array[0] << 1));
      int8_t wave_1_main   = get_wave_level(m_wave_table[1], (m_phase_array[1] << 1));
      int8_t wave_2_main   = get_wave_level(m_wave_table[2], (m_phase_array[2] << 1));
      int8_t wave_0_detune = get_wave_level(m_wave_table[0], (m_phase_array[0] << 1) + (m_phase_detune << 1));
      int8_t wave_1_detune = get_wave_level(m_wave_table[1], (m_phase_array[1] << 1) + (m_phase_detune << 1));
      int8_t wave_2_detune = get_wave_level(m_wave_table[2], (m_phase_array[2] << 1) + (m_phase_detune << 1));
      int8_t wave_0_sub    = get_tri_wave_level(m_phase_array[0]);
      int8_t wave_1_sub    = get_tri_wave_level(m_phase_array[1]);
      int8_t wave_2_sub    = get_tri_wave_level(m_phase_array[2]);

      // amp and mix
      int16_t level_main   = mul_q15_q8((wave_0_main   * amp_0) +
                                        (wave_1_main   * amp_1) +
                                        (wave_2_main   * amp_2), m_mix_main);
      int16_t level_detune = mul_q15_q8((wave_0_detune * amp_0) +
                                        (wave_1_detune * amp_1) +
                                        (wave_2_detune * amp_2), m_mix_detune);
      int16_t level_sub    = mul_q15_q8((wave_0_sub    * amp_0) +
                                        (wave_1_sub    * amp_1) +
                                        (wave_2_sub    * amp_2), m_sub);

      result = level_main + level_detune + level_sub;
    }

    return result;
  }

private:
  INLINE static const uint8_t* get_wave_table(uint8_t waveform, uint8_t note_number) {
    const uint8_t* result;
    if (waveform == OSC_WAVEFORM_SAW) {
      result = g_osc_saw_wave_tables[note_number - NOTE_NUMBER_MIN];
    } else if (waveform == OSC_WAVEFORM_ORGAN) {
      result = g_osc_org9_wave_tables[note_number - NOTE_NUMBER_MIN];
    } else {
      result = g_osc_sq_wave_tables[note_number - NOTE_NUMBER_MIN];
    }
    return result;
  }

  INLINE static int8_t get_wave_level(const uint8_t* wave_table, __uint24 phase) {
    uint8_t curr_index = hhigh_byte(phase);
    uint8_t next_weight = high_byte(phase);
    uint16_t two_data = pgm_read_word(wave_table + curr_index);
    uint8_t curr_data = low_byte(two_data);
    uint8_t next_data = high_byte(two_data);

    // lerp
    int8_t result;
    if (static_cast<int8_t>(curr_data) < static_cast<int8_t>(next_data)) {
      result = curr_data + high_byte(static_cast<uint8_t>(next_data - curr_data) * next_weight);
    } else {
      result = curr_data - high_byte(static_cast<uint8_t>(curr_data - next_data) * next_weight);
    }

    return result;
  }

  INLINE static int8_t get_tri_wave_level(__uint24 phase) {
    uint16_t level = phase >> 8;
    if ((level & 0x8000) != 0) {
      level = ~level;
    }
    level -= 0x4000;
    return high_sbyte(level << 1);
  }
};

template <uint8_t T> boolean         Osc<T>::m_unison_on;
template <uint8_t T> uint8_t         Osc<T>::m_mix_main;
template <uint8_t T> uint8_t         Osc<T>::m_mix_detune;
template <uint8_t T> uint8_t         Osc<T>::m_mix_table[OSC_MIX_TABLE_LENGTH];
template <uint8_t T> uint8_t         Osc<T>::m_waveform;
template <uint8_t T> uint8_t         Osc<T>::m_sub;
template <uint8_t T> const uint8_t*  Osc<T>::m_wave_table[3];
template <uint8_t T> __uint24        Osc<T>::m_freq_array[3];
template <uint8_t T> uint16_t        Osc<T>::m_freq_detune;
template <uint8_t T> __uint24        Osc<T>::m_phase_array[3];
template <uint8_t T> __uint24        Osc<T>::m_phase_detune;
