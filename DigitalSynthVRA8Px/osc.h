#pragma once

#include "common.h"
#include "osc-table.h"

template <uint8_t T>
class Osc {
  static boolean        m_unison_on;
  static uint8_t        m_waveform;
  static uint16_t       m_base_detune;
  static const uint8_t* m_wave_table[3];
  static __uint24       m_freq_array[3];
  static uint16_t       m_freq_detune;
  static __uint24       m_phase_array[3];
  static __uint24       m_phase_detune;

public:
  INLINE static void initialize() {
    m_unison_on = false;
    m_waveform = 0;
    m_base_detune = 0;
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

  INLINE static void set_waveform(uint8_t waveform) {
    m_waveform = waveform;
  }

  INLINE static void set_detune(uint8_t controller_value) {
    m_base_detune = ((uint16_t) high_byte((controller_value << 1) *
                                          (controller_value << 1)) <<
                     OSC_DETUNE_MUL_NUM_BITS) + 255;

    m_freq_detune = m_base_detune;
    if (m_waveform == OSC_WAVEFORM_ORGAN) {
      m_freq_detune = (m_freq_detune + 1) >> 1;
    }

    if (m_unison_on) {
      m_freq_array[1] = m_freq_array[0] + (m_freq_detune << 1);
      m_freq_array[2] = m_freq_array[0] - (m_freq_detune << 1);
    }
  }

  INLINE static void note_on(uint8_t osc_number, uint8_t note_number) {
    m_freq_detune = m_base_detune;
    if (m_waveform == OSC_WAVEFORM_ORGAN) {
      m_freq_detune = (m_freq_detune + 1) >> 1;
    }

    if (m_unison_on) {
      m_wave_table[0] = get_wave_table(m_waveform, note_number);
      m_wave_table[1] = m_wave_table[0];
      m_wave_table[2] = m_wave_table[0];
      m_freq_array[0] = get_freq(m_waveform, note_number);
      m_freq_array[1] = m_freq_array[0] + (m_freq_detune << 1);
      m_freq_array[2] = m_freq_array[0] - (m_freq_detune << 1);
    } else {
      m_wave_table[osc_number] = get_wave_table(m_waveform, note_number);
      m_freq_array[osc_number] = get_freq(m_waveform, note_number);
    }
  }

  INLINE static int16_t clock(uint8_t amp_0, uint8_t amp_1, uint8_t amp_2) {
    m_phase_array[0] += m_freq_array[0];
    m_phase_array[1] += m_freq_array[1];
    m_phase_array[2] += m_freq_array[2];
    m_phase_detune += m_freq_detune;

    int8_t wave_0_0 = get_wave_level(m_wave_table[0], m_phase_array[0]);
    int8_t wave_1_0 = get_wave_level(m_wave_table[1], m_phase_array[1]);
    int8_t wave_2_0 = get_wave_level(m_wave_table[2], m_phase_array[2]);
    int8_t wave_0_1 = get_wave_level(m_wave_table[0], m_phase_array[0] + m_phase_detune);
    int8_t wave_1_1 = get_wave_level(m_wave_table[1], m_phase_array[1] + m_phase_detune);
    int8_t wave_2_1 = get_wave_level(m_wave_table[2], m_phase_array[2] + m_phase_detune);

    // amp and mix
    int16_t result = (wave_0_0 * (amp_0 + (amp_0 >> 1))) + (wave_0_1 * amp_0) +
                     (wave_1_0 * (amp_1 + (amp_1 >> 1))) + (wave_1_1 * amp_1) +
                     (wave_2_0 * (amp_2 + (amp_2 >> 1))) + (wave_2_1 * amp_2);
    result >>= 1;

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

  INLINE static __uint24 get_freq(uint8_t waveform, uint8_t note_number) {
    __uint24 result;
    result = g_osc_freq_table[note_number - NOTE_NUMBER_MIN];
    if (waveform == OSC_WAVEFORM_ORGAN) {
      result = ((result + 1) >> 1) - 1;
    }
    return result;
  }

  INLINE static int8_t get_wave_level(const uint8_t* wave_table, __uint24 phase) {
    uint8_t curr_index = hhigh_byte(phase);
    uint8_t next_weight = high_byte(phase);
    __uint24 two_data = pgm_read_word(wave_table + curr_index);
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
};

template <uint8_t T> boolean         Osc<T>::m_unison_on;
template <uint8_t T> uint8_t         Osc<T>::m_waveform;
template <uint8_t T> uint16_t        Osc<T>::m_base_detune;
template <uint8_t T> const uint8_t*  Osc<T>::m_wave_table[3];
template <uint8_t T> __uint24        Osc<T>::m_freq_array[3];
template <uint8_t T> uint16_t        Osc<T>::m_freq_detune;
template <uint8_t T> __uint24        Osc<T>::m_phase_array[3];
template <uint8_t T> __uint24        Osc<T>::m_phase_detune;
