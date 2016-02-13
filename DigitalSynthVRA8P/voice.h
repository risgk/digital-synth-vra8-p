#include "common.h"

template <uint8_t T>
class Voice {
  static boolean m_mono;
  static uint8_t m_note_number[3];

public:
  INLINE static void initialize() {
    m_note_number[0] = NOTE_NUMBER_INVALID;
    m_note_number[1] = NOTE_NUMBER_INVALID;
    m_note_number[2] = NOTE_NUMBER_INVALID;
    IOsc<0>::initialize();
    IFilter<0>::initialize();
    IAmp<0>::initialize();
    IEG<0>::initialize();
    m_mono = false;
  }

  INLINE static void set_poly_mono(uint8_t controller_value) {
    if (!m_mono && controller_value >= 64) {
      m_mono = true;
      all_note_off();
    } else if (m_mono && controller_value < 64) {
      m_mono = false;
      all_note_off();
    }
  }

  INLINE static void note_on(uint8_t note_number) {
    if ((note_number < NOTE_NUMBER_MIN) ||
        (note_number > NOTE_NUMBER_MAX)) {
      return;
    }

    if (m_mono) {
      m_note_number[0] = note_number;
      IOsc<0>::note_on(0, note_number);
      IOsc<0>::note_on(1, note_number);
      IOsc<0>::note_on(2, note_number);
      IEG<0>::note_on(0);
      IEG<0>::note_on(1);
      IEG<0>::note_on(2);
    } else {
      if (m_note_number[0] == NOTE_NUMBER_INVALID) {
        m_note_number[0] = note_number;
        IOsc<0>::note_on(0, note_number);
        IEG<0>::note_on(0);
      } else if (m_note_number[1] == NOTE_NUMBER_INVALID) {
        m_note_number[1] = note_number;
        IOsc<0>::note_on(1, note_number);
        IEG<0>::note_on(1);
      } else {
        m_note_number[2] = note_number;
        IOsc<0>::note_on(2, note_number);
        IEG<0>::note_on(2);
      }
    }

    IEG<0>::note_on(3);
  }

  INLINE static void note_off(uint8_t note_number) {
    if (m_mono) {
      if (m_note_number[0] == note_number) {
        all_note_off();
      }
    } else {
      if (m_note_number[0] == note_number) {
        m_note_number[0] = NOTE_NUMBER_INVALID;
        IEG<0>::note_off(0);
      } else if (m_note_number[1] == note_number) {
        m_note_number[1] = NOTE_NUMBER_INVALID;
        IEG<0>::note_off(1);
      } else if (m_note_number[2] == note_number) {
        m_note_number[2] = NOTE_NUMBER_INVALID;
        IEG<0>::note_off(2);
      }
    }

    if ((m_note_number[0] == NOTE_NUMBER_INVALID) &&
        (m_note_number[1] == NOTE_NUMBER_INVALID) &&
        (m_note_number[2] == NOTE_NUMBER_INVALID)) {
      IEG<0>::note_off(3);
    }
  }

  INLINE static void all_note_off() {
    m_note_number[0] = NOTE_NUMBER_INVALID;
    IEG<0>::note_off(0);
    m_note_number[1] = NOTE_NUMBER_INVALID;
    IEG<0>::note_off(1);
    m_note_number[2] = NOTE_NUMBER_INVALID;
    IEG<0>::note_off(2);
  }

  INLINE static void control_change(uint8_t controller_number, uint8_t controller_value) {
    switch (controller_number) {
    case OSC_POLY_MONO:
      set_poly_mono(controller_value);
      IOsc<0>::set_poly_mono(controller_value);
      break;
    case OSC_SAW_SQ:
      IOsc<0>::set_saw_sq(controller_value);
      break;
    case OSC_DETUNE:
      IOsc<0>::set_detune(controller_value);
      break;
    case AMP_GATE_ENV:
      // todo
      break;
    case LPF_CUTOFF:
      IFilter<0>::set_cutoff(controller_value);
      break;
    case LPF_RESONANCE:
      IFilter<0>::set_resonance(controller_value);
      break;
    case LPF_ENV_AMT:
      IFilter<0>::set_env_amt(controller_value);
      break;
    case ENV_D_S_A:
      IEG<0>::set_attack(controller_value);
      IEG<0>::set_release(controller_value);
      break;
    }
  }

  INLINE static int8_t clock() {
    uint8_t eg_output[4];
    IEG<0>::clock();
    eg_output[0] = IEG<0>::level<0>();
    eg_output[1] = IEG<0>::level<1>();
    eg_output[2] = IEG<0>::level<2>();
    eg_output[3] = IEG<0>::level<3>();
    int16_t osc_output    = IOsc<0>::clock(eg_output[0],
                                           eg_output[1],
                                           eg_output[2]);
    int16_t filter_output = IFilter<0>::clock(osc_output, eg_output[3]);
    int16_t amp_output    = IAmp<0>::clock(filter_output, eg_output[0]);  // TODO
    return high_sbyte(amp_output);
  }
};

template <uint8_t T> boolean Voice<T>::m_mono;
template <uint8_t T> uint8_t Voice<T>::m_note_number[3];
