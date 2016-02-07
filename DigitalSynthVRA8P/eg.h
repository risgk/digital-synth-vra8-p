#pragma once

#include "common.h"
#include "mul-q.h"
#include "eg-table.h"

template <uint8_t T>
class EG {
  static uint8_t  m_count;
  static bool     m_gate[4];
  static uint16_t m_level[4];
  static uint16_t m_attack_rate;
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
    m_attack_rate =
      g_eg_attack_rate_table[controller_value >> (7 - EG_CONTROLLER_STEPS_BITS)];
  }

  INLINE static void set_release(uint8_t controller_value) {
    uint8_t decay_time;
    uint8_t release_time;

    uint8_t decay_value;
    if (controller_value < 64) {
      decay_time = controller_value >> (6 - EG_CONTROLLER_STEPS_BITS);
      release_time = controller_value >> (6 - EG_CONTROLLER_STEPS_BITS);
    } else {
      decay_time = 64 >> (6 - EG_CONTROLLER_STEPS_BITS);
      release_time = (127 - controller_value) >> (6 - EG_CONTROLLER_STEPS_BITS);
    }

    if (decay_time == (64 >> (6 - EG_CONTROLLER_STEPS_BITS))) {
      m_release_rate = 0;
    } else {
      m_release_rate = g_eg_decay_release_rate_table[decay_time];
    }

    m_release_rate = g_eg_decay_release_rate_table[release_time];
  }

  INLINE static void note_on(uint8_t eg_number) {
    m_gate[eg_number] = true;
  }

  INLINE static void note_off(uint8_t eg_number) {
    m_gate[eg_number] = false;
  }

  INLINE static void clock() {
    m_count++;
    switch (m_count >> 1) {
    case 0:
      sub_clock<0>();
      break;
    case 1:
      sub_clock<1>();
      break;
    case 2:
      sub_clock<2>();
      break;
    case 3:
      sub_clock<3>();
      break;
    }
#if 0
          m_level -= high_byte(m_level) * static_cast<uint8_t>(-m_release_rate);
#endif
  }

  template <uint8_t N>
  INLINE static uint8_t level() {
    return (m_level[N] >> 8);
  }

private:
  template <uint8_t N>
  INLINE static void sub_clock() {
    if (m_gate[N]) {
      m_level[N] = EG_LEVEL_MAX;
    } else {
      m_level[N] = 0;
    }
  }
};

template <uint8_t T> uint8_t  EG<T>::m_count;
template <uint8_t T> bool     EG<T>::m_gate[4];
template <uint8_t T> uint16_t EG<T>::m_level[4];
template <uint8_t T> uint16_t EG<T>::m_attack_rate;
template <uint8_t T> uint8_t  EG<T>::m_release_rate;
