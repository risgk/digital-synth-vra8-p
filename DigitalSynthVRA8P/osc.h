#pragma once

#include "common.h"
#include "osc-table.h"

template <uint8_t T>
class Osc {
  static boolean        m_mono;
  static uint8_t        m_waveform;
  static const uint8_t* m_wave_table[3];
  static uint16_t       m_freq[3];
  static uint8_t        m_freq_detune;
  static uint16_t       m_phase[3];
  static uint16_t       m_phase_detune;

public:
  INLINE static void initialize() {
    m_mono = false;
    m_waveform = 0;
    m_wave_table[0] = g_osc_saw_wave_tables[0];
    m_wave_table[1] = g_osc_saw_wave_tables[2];
    m_wave_table[2] = g_osc_saw_wave_tables[4];
    m_freq[0] = g_osc_freq_table[0];
    m_freq[1] = g_osc_freq_table[2];
    m_freq[2] = g_osc_freq_table[4];
    m_freq_detune = 0;
    m_phase[0] = 0;
    m_phase[1] = 0;
    m_phase[2] = 0;
    m_phase_detune = 0;
  }

  INLINE static void set_mono(uint8_t controller_value) {
    if (controller_value >= 64) {
      m_mono = true;
    } else {
      m_mono = false;
    }
  }

  INLINE static void set_waveform(uint8_t controller_value) {
    m_waveform = controller_value;
  }

  INLINE static void set_detune(uint8_t controller_value) {
    m_freq_detune = (controller_value >> 4) + 1;
    if (m_mono) {
      m_freq[1] = m_freq[0] + (m_freq_detune << 1);
      m_freq[2] = m_freq[0] - (m_freq_detune << 1);
    }
  }

  INLINE static void note_on(uint8_t osc_number, uint8_t note_number) {
    if (m_mono) {
      if (m_waveform < 96) {
        m_wave_table[0] = g_osc_saw_wave_tables[note_number - NOTE_NUMBER_MIN];
      } else {
        m_wave_table[0] = g_osc_sq_wave_tables[note_number - NOTE_NUMBER_MIN];
      }
      m_wave_table[1] = m_wave_table[0];
      m_wave_table[2] = m_wave_table[0];
      m_freq[0] = g_osc_freq_table[note_number - NOTE_NUMBER_MIN];
      m_freq[1] = m_freq[0] + (m_freq_detune << 1);
      m_freq[2] = m_freq[0] - (m_freq_detune << 1);
    } else {
      if (m_waveform < 96) {
        m_wave_table[osc_number] = g_osc_saw_wave_tables[note_number - NOTE_NUMBER_MIN];
      } else {
        m_wave_table[osc_number] = g_osc_sq_wave_tables[note_number - NOTE_NUMBER_MIN];
      }
      m_freq[osc_number] = g_osc_freq_table[note_number - NOTE_NUMBER_MIN];
    }
  }

  INLINE static int16_t clock(uint8_t amp_0, uint8_t amp_1, uint8_t amp_2) {
    m_phase[0] += m_freq[0];
    m_phase[1] += m_freq[1];
    m_phase[2] += m_freq[2];
    m_phase_detune += m_freq_detune;

    int8_t wave_0_0 = get_wave_level(m_wave_table[0], m_phase[0]);
    int8_t wave_1_0 = get_wave_level(m_wave_table[1], m_phase[1]);
    int8_t wave_2_0 = get_wave_level(m_wave_table[2], m_phase[2]);
    int8_t wave_0_1 = get_wave_level(m_wave_table[0], m_phase[0] + m_phase_detune);
    int8_t wave_1_1 = get_wave_level(m_wave_table[1], m_phase[1] + m_phase_detune);
    int8_t wave_2_1 = get_wave_level(m_wave_table[2], m_phase[2] + m_phase_detune);

    // amp and mix
    int16_t result = (wave_0_0 * amp_0) + (wave_0_1 * amp_0) +
                     (wave_1_0 * amp_1) + (wave_1_1 * amp_1) +
                     (wave_2_0 * amp_2) + (wave_2_1 * amp_2);

    return result;
  }

private:
  INLINE static int8_t get_wave_level(const uint8_t* wave_table, uint16_t phase) {
    uint8_t curr_index = high_byte(phase);
    uint16_t two_data = pgm_read_word(wave_table + curr_index);
    uint8_t curr_data = low_byte(two_data);
    uint8_t next_data = high_byte(two_data);
    uint8_t next_weight = low_byte(phase);

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

template <uint8_t T> boolean         Osc<T>::m_mono;
template <uint8_t T> uint8_t         Osc<T>::m_waveform;
template <uint8_t T> const uint8_t*  Osc<T>::m_wave_table[3];
template <uint8_t T> uint16_t        Osc<T>::m_freq[3];
template <uint8_t T> uint8_t         Osc<T>::m_freq_detune;
template <uint8_t T> uint16_t        Osc<T>::m_phase[3];
template <uint8_t T> uint16_t        Osc<T>::m_phase_detune;
