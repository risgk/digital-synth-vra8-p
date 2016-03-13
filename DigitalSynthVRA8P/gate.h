#pragma once

#include "common.h"
#include "mul-q.h"

template <uint8_t T>
class Gate {
  static boolean m_gate_array[4];
  static uint8_t m_level_array[4];
  static uint8_t m_count;

public:
  INLINE static void initialize() {
    m_gate_array[0] = false;
    m_gate_array[1] = false;
    m_gate_array[2] = false;
    m_gate_array[3] = false;
    m_level_array[0] = 0;
    m_level_array[1] = 0;
    m_level_array[2] = 0;
    m_level_array[3] = 0;
    m_count = 0;
  }

  INLINE static void note_on(uint8_t eg_number) {
    m_gate_array[eg_number] = true;
  }

  INLINE static void note_off(uint8_t eg_number) {
    m_gate_array[eg_number] = false;
  }

  INLINE static void clock() {
    m_count++;
    if ((m_count & (GATE_CONTROL_INTERVAL - 1)) == 0) {
      update_level<0>();
      update_level<1>();
      update_level<2>();
      update_level<3>();
    }
  }

  template <uint8_t N>
  INLINE static uint8_t level() {
    return m_level_array[N];
  }

private:
  template <uint8_t N>
  INLINE static void update_level() {
    if (m_gate_array[N] && (m_level_array[N] < GATE_LEVEL_MAX)) {
      m_level_array[N] += GATE_LEVEL_STEP;
    } else if (!m_gate_array[N] && (m_level_array[N] > 0)) {
      m_level_array[N] -= GATE_LEVEL_STEP;
    }
  }
};

template <uint8_t T> boolean Gate<T>::m_gate_array[4];
template <uint8_t T> uint8_t Gate<T>::m_level_array[4];
template <uint8_t T> uint8_t Gate<T>::m_count;
