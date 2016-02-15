#include "common.h"

template <uint8_t T>
class Voice {
  static boolean m_mono;
  static boolean m_amp_env;
  static uint8_t m_note_number[3];

public:
  INLINE static void initialize() {
    m_mono = false;
    m_amp_env = true;
    m_note_number[0] = NOTE_NUMBER_INVALID;
    m_note_number[1] = NOTE_NUMBER_INVALID;
    m_note_number[2] = NOTE_NUMBER_INVALID;
    IOsc<0>::initialize();
    IFilter<0>::initialize();
    IAmp<0>::initialize();
    IGate<0>::initialize();
    IEnvGen<0>::initialize();
  }

  INLINE static void set_mono(uint8_t controller_value) {
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
      IGate<0>::note_on(0);
      IGate<0>::note_on(1);
      IGate<0>::note_on(2);
    } else {
      if (m_note_number[0] == NOTE_NUMBER_INVALID) {
        m_note_number[0] = note_number;
        IOsc<0>::note_on(0, note_number);
        IGate<0>::note_on(0);
      } else if (m_note_number[1] == NOTE_NUMBER_INVALID) {
        m_note_number[1] = note_number;
        IOsc<0>::note_on(1, note_number);
        IGate<0>::note_on(1);
      } else {
        m_note_number[2] = note_number;
        IOsc<0>::note_on(2, note_number);
        IGate<0>::note_on(2);
      }
    }

    IGate<0>::note_on(3);
    IEnvGen<0>::note_on();
  }

  INLINE static void note_off(uint8_t note_number) {
    if (m_mono) {
      if (m_note_number[0] == note_number) {
        all_note_off();
      }
    } else {
      if (m_note_number[0] == note_number) {
        m_note_number[0] = NOTE_NUMBER_INVALID;
        IGate<0>::note_off(0);
      } else if (m_note_number[1] == note_number) {
        m_note_number[1] = NOTE_NUMBER_INVALID;
        IGate<0>::note_off(1);
      } else if (m_note_number[2] == note_number) {
        m_note_number[2] = NOTE_NUMBER_INVALID;
        IGate<0>::note_off(2);
      }
    }

    if ((m_note_number[0] == NOTE_NUMBER_INVALID) &&
        (m_note_number[1] == NOTE_NUMBER_INVALID) &&
        (m_note_number[2] == NOTE_NUMBER_INVALID)) {
      IGate<0>::note_off(3);
      IEnvGen<0>::note_off();
    }
  }

  INLINE static void all_note_off() {
    m_note_number[0] = NOTE_NUMBER_INVALID;
    IGate<0>::note_off(0);
    m_note_number[1] = NOTE_NUMBER_INVALID;
    IGate<0>::note_off(1);
    m_note_number[2] = NOTE_NUMBER_INVALID;
    IGate<0>::note_off(2);
    IGate<0>::note_off(3);
    IEnvGen<0>::note_off();
  }

  INLINE static void control_change(uint8_t controller_number, uint8_t controller_value) {
    switch (controller_number) {
    case OSC_POLY_MONO:
      set_mono(controller_value);
      IOsc<0>::set_mono(controller_value);
      break;
    case OSC_SAW_SQ:
      IOsc<0>::set_waveform(controller_value);
      break;
    case OSC_DETUNE:
      IOsc<0>::set_detune(controller_value);
      break;
    case AMP_GATE_ENV:
      if (controller_value < 64) {
        m_amp_env = false;
      } else  {
        m_amp_env = true;
      }
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
      if (controller_value < 32) {
        IEnvGen<0>::set_decay(controller_value << 2);
        IEnvGen<0>::set_sustain(0);
        IEnvGen<0>::set_attack(0);
      } else if (controller_value < 64) {
        IEnvGen<0>::set_decay(124);
        IEnvGen<0>::set_sustain((controller_value - 32) << 2);
        IEnvGen<0>::set_attack(0);
      } else  {
        IEnvGen<0>::set_decay(124);
        IEnvGen<0>::set_sustain(124);
        IEnvGen<0>::set_attack((controller_value - 64) << 1);
      }
      break;
    }
  }

  INLINE static int8_t clock() {
    uint8_t gate_output[4];
    IGate<0>::clock();
    gate_output[0] = IGate<0>::level<0>();
    gate_output[1] = IGate<0>::level<1>();
    gate_output[2] = IGate<0>::level<2>();
    int16_t osc_output = IOsc<0>::clock(gate_output[0], gate_output[1],
                                        gate_output[2]);
    uint8_t env_gen_output = IEnvGen<0>::clock();
    int16_t filter_output = IFilter<0>::clock(osc_output, env_gen_output);
    int16_t amp_output;
    if (m_amp_env) {
      amp_output = IAmp<0>::clock(filter_output, env_gen_output);
    } else {
      gate_output[3] = IGate<0>::level<3>();
      amp_output = IAmp<0>::clock(filter_output, gate_output[3] << 2);
    }
    return high_sbyte(amp_output);
  }
};

template <uint8_t T> boolean Voice<T>::m_mono;
template <uint8_t T> boolean Voice<T>::m_amp_env;
template <uint8_t T> uint8_t Voice<T>::m_note_number[3];
