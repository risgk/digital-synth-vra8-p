#pragma once

#include "common.h"
#include "mul-q.h"

template <uint8_t T>
class Gate {
  static bool     m_gate[4];
  static uint8_t  m_level[4];
  static uint8_t  m_count;

public:
  INLINE static void initialize() {
    m_gate[0] = false;
    m_gate[1] = false;
    m_gate[2] = false;
    m_gate[3] = false;
    m_level[0] = 0;
    m_level[1] = 0;
    m_level[2] = 0;
    m_level[3] = 0;
    m_count = 0;
  }

  INLINE static void note_on(uint8_t eg_number) {
    m_gate[eg_number] = true;
  }

  INLINE static void note_off(uint8_t eg_number) {
    m_gate[eg_number] = false;
  }

  INLINE static void clock() {
    m_count++;
    switch (m_count & 0x0F) {
    case 0:
      sub_clock<0>();
      break;
    case 4:
      sub_clock<1>();
      break;
    case 8:
      sub_clock<2>();
      break;
    case 12:
      sub_clock<3>();
      break;
    }
  }

  template <uint8_t N>
  INLINE static uint8_t level() {
    return m_level[N];
  }

private:
  template <uint8_t N>
  INLINE static void sub_clock() {
    if (m_gate[N]) {
      if (m_level[N] < 255) {
        m_level[N] += 15;
      }
    } else if (m_level[N] > 0) {
      m_level[N] -= 15;
    }
  }
};

template <uint8_t T> bool     Gate<T>::m_gate[4];
template <uint8_t T> uint8_t  Gate<T>::m_level[4];
template <uint8_t T> uint8_t  Gate<T>::m_count;
