#pragma once

#include "common.h"
#include "mul-q.h"
#include "eg-table.h"

template <uint8_t T>
class EG {
  static uint8_t  m_count;
  static bool     m_gate[4];
  static uint8_t  m_level[4];
  static uint8_t  m_attack_rate;
  static uint8_t  m_release_rate;

public:
  INLINE static void initialize() {
    m_count = 0;
    m_gate[0] = false;
    m_gate[1] = false;
    m_gate[2] = false;
    m_gate[3] = false;
    m_level[0] = 0;
    m_level[1] = 0;
    m_level[2] = 0;
    m_level[3] = 0;
    set_attack(0);
    set_release(127);
  }

  INLINE static void set_attack(uint8_t controller_value) {
    m_attack_rate = controller_value + 127;
  }

  INLINE static void set_release(uint8_t controller_value) {
    m_release_rate = controller_value + 127;
  }

  INLINE static void note_on(uint8_t eg_number) {
    m_gate[eg_number] = true;
  }

  INLINE static void note_off(uint8_t eg_number) {
    m_gate[eg_number] = false;
  }

  INLINE static void clock() {
    m_count++;
    switch (m_count) {
    case 0:
      sub_clock<0>();
      break;
    case 64:
      sub_clock<1>();
      break;
    case 128:
      sub_clock<2>();
      break;
    case 192:
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
      m_level[N] = ~high_byte(static_cast<uint8_t>(~m_level[N]) *
                                                   m_attack_rate);
    } else {
      m_level[N] = high_byte(m_level[N] * m_release_rate);
    }
  }
};

template <uint8_t T> uint8_t  EG<T>::m_count;
template <uint8_t T> bool     EG<T>::m_gate[4];
template <uint8_t T> uint8_t  EG<T>::m_level[4];
template <uint8_t T> uint8_t  EG<T>::m_attack_rate;
template <uint8_t T> uint8_t  EG<T>::m_release_rate;
