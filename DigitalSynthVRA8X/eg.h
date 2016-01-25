#pragma once

#include "common.h"
#include "mul-q.h"
#include "eg-table.h"

template <uint8_t T>
class EG {
  static const uint8_t  STATE_ATTACK  = 0;
  static const uint8_t  STATE_DECAY   = 1;
  static const uint8_t  STATE_RELEASE = 2;
  static const uint8_t  STATE_IDLE    = 3;

  static uint8_t  m_state;
  static uint16_t m_count;
  static uint16_t m_level;
  static uint16_t m_attack_rate;
  static uint8_t  m_decay_rate;
  static uint8_t  m_release_rate;
  static uint16_t m_decay_update_interval;
  static uint16_t m_release_update_interval;

public:
  INLINE static void initialize() {
    m_state = STATE_IDLE;
    m_count = 0;
    m_level = 0;
    set_attack(0);
    set_decay_release(127);
  }

  INLINE static void set_attack(uint8_t controller_value) {
    m_attack_rate =
      g_eg_attack_rate_table[controller_value >> (7 - EG_CONTROLLER_STEPS_BITS)];
  }

  INLINE static void set_decay_release(uint8_t controller_value) {
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
      m_decay_rate = 0;
      m_decay_update_interval = 0;
    } else {
      m_decay_rate = g_eg_decay_release_rate_table[decay_time];
      m_decay_update_interval = g_eg_decay_release_update_interval_table[decay_time];
    }

    m_release_rate = g_eg_decay_release_rate_table[release_time];
    m_release_update_interval = g_eg_decay_release_update_interval_table[release_time];
  }

  INLINE static void note_on() {
    m_state = STATE_ATTACK;
    m_count = EG_ATTACK_UPDATE_INTERVAL;
  }

  INLINE static void note_off() {
    m_state = STATE_RELEASE;
    m_count = m_release_update_interval;
  }

  INLINE static int8_t clock() {
    switch (m_state) {
    case STATE_ATTACK:
      m_count--;
      if (m_count == 0) {
        m_count = EG_ATTACK_UPDATE_INTERVAL;
        if (m_level >= EG_LEVEL_MAX - m_attack_rate) {
          m_state = STATE_DECAY;
          m_level = EG_LEVEL_MAX;
        } else {
          m_level += m_attack_rate;
        }
      }
      break;
    case STATE_DECAY:
      if (m_decay_update_interval != 0) {
        m_count--;
        if (m_count == 0) {
          m_count = m_decay_update_interval;
          if (m_level < 256) {
            m_state = STATE_IDLE;
            m_level = 0;
          } else {
            m_level -= high_byte(m_level) * static_cast<uint8_t>(-m_decay_rate);
          }
        }
      }
      break;
    case STATE_RELEASE:
      m_count--;
      if (m_count == 0) {
        m_count = m_release_update_interval;
        if (m_level < 256) {
          m_state = STATE_IDLE;
          m_level = 0;
        } else {
          m_level -= high_byte(m_level) * static_cast<uint8_t>(-m_release_rate);
        }
      }
      break;
    case STATE_IDLE:
      m_level = 0;
      break;
    }
    return high_byte(m_level);
  }
};

template <uint8_t T> uint8_t  EG<T>::m_state;
template <uint8_t T> uint16_t EG<T>::m_count;
template <uint8_t T> uint16_t EG<T>::m_level;
template <uint8_t T> uint16_t EG<T>::m_attack_rate;
template <uint8_t T> uint8_t  EG<T>::m_decay_rate;
template <uint8_t T> uint8_t  EG<T>::m_release_rate;
template <uint8_t T> uint16_t EG<T>::m_decay_update_interval;
template <uint8_t T> uint16_t EG<T>::m_release_update_interval;
