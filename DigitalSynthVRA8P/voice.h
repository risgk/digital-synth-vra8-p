#include "common.h"

template <uint8_t T>
class Voice {
  static uint8_t m_note_number[3];

public:
  INLINE static void initialize() {
    m_note_number[0] = 0xFF;
    m_note_number[1] = 0xFF;
    m_note_number[2] = 0xFF;
    IOsc<0>::initialize();
    IFilter<0>::initialize();
    IAmp<0>::initialize();
    IEG<0>::initialize();
  }

  INLINE static void note_on(uint8_t note_number) {
    if ((note_number < NOTE_NUMBER_MIN) || (note_number > NOTE_NUMBER_MAX)) {
      return;
    }

    if (m_note_number[0] == 0xFF) {
      IOsc<0>::note_off(0);
    }
    if (m_note_number[1] == 0xFF) {
      IOsc<0>::note_off(1);
    }
    if (m_note_number[2] == 0xFF) {
      IOsc<0>::note_off(2);
    }

    if (m_note_number[0] == 0xFF) {
      m_note_number[0] = note_number;
      IOsc<0>::note_on(0, note_number);
    } else if (m_note_number[1] == 0xFF) {
      m_note_number[1] = note_number;
      IOsc<0>::note_on(1, note_number);
    } else {
      m_note_number[2] = note_number;
      IOsc<0>::note_on(2, note_number);
    }

    IEG<0>::note_on();
  }

  INLINE static void note_off(uint8_t note_number) {
    if (m_note_number[0] == note_number) {
      m_note_number[0] = 0xFF;
    } else if (m_note_number[1] == note_number) {
      m_note_number[1] = 0xFF;
    } else if (m_note_number[2] == note_number) {
      m_note_number[2] = 0xFF;
    }

    if (m_note_number[0] == 0xFF && m_note_number[1] == 0xFF &&
        m_note_number[2] == 0xFF) {
      IEG<0>::note_off();
    }
  }

  INLINE static void all_note_off() {
    IEG<0>::note_off();
  }

  INLINE static void control_change(uint8_t controller_number, uint8_t controller_value) {
    switch (controller_number) {
    case OSC_MODE:
      IOsc<0>::set_mode(controller_value);
      break;
    case OSC_COLOR:
      IOsc<0>::set_color(controller_value);
      break;
    case MOD_RATE:
      IOsc<0>::set_mod_rate(controller_value);
      break;
    case MOD_DEPTH:
      IOsc<0>::set_mod_depth(controller_value);
      break;
    case LPF_CUTOFF_ENV:
      IFilter<0>::set_cutoff(controller_value);
      break;
    case LPF_RESONANCE:
      IFilter<0>::set_resonance(controller_value);
      break;
    case ENV_A:
      IEG<0>::set_attack(controller_value);
      break;
    case ENV_D_R:
      IEG<0>::set_decay_release(controller_value);
      break;
    }
  }

  INLINE static int8_t clock() {
    uint8_t eg_output     = IEG<0>::clock();
    int16_t osc_output    = IOsc<0>::clock(eg_output);
    int16_t filter_output = IFilter<0>::clock(osc_output, eg_output);
    int16_t amp_output    = IAmp<0>::clock(filter_output, eg_output);
    return high_sbyte(amp_output);
  }
};

template <uint8_t T> uint8_t Voice<T>::m_note_number[3];
