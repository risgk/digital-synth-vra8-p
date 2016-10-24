#include "common.h"

template <uint8_t T>
class Voice {
  static boolean m_unison_on;
  static uint8_t m_waveform;
  static uint8_t m_amp_env_amt;
  static uint8_t m_note_number[3];
  static uint8_t m_output_error;

public:
  INLINE static void initialize() {
    m_unison_on = false;
    m_waveform = OSC_WAVEFORM_SAW;
    m_amp_env_amt = 127 << 1;
    m_note_number[0] = NOTE_NUMBER_INVALID;
    m_note_number[1] = NOTE_NUMBER_INVALID;
    m_note_number[2] = NOTE_NUMBER_INVALID;
    m_output_error = 0;
    IOsc<0>::initialize();
    IFilter<0>::initialize();
    IAmp<0>::initialize();
    IGate<0>::initialize();
    IEnvGen<0>::initialize();
  }

  INLINE static void set_unison(uint8_t controller_value) {
    if (controller_value >= 64) {
      IOsc<0>::set_mix((127 - controller_value) << 1);
      if (!m_unison_on) {
        m_unison_on = true;
        IOsc<0>::set_unison(m_unison_on);
        if (m_note_number[0] != NOTE_NUMBER_INVALID) {
          m_note_number[1] = NOTE_NUMBER_INVALID;
          m_note_number[2] = NOTE_NUMBER_INVALID;
          IOsc<0>::note_on(0, m_note_number[0]);
          IGate<0>::note_on(1);
          IGate<0>::note_on(2);
        } else {
          all_note_off();
        }
      }
    } else {
      IOsc<0>::set_mix(controller_value << 1);
      if (m_unison_on) {
        m_unison_on = false;
        IOsc<0>::set_unison(m_unison_on);
        if (m_note_number[0] != NOTE_NUMBER_INVALID) {
          IOsc<0>::note_on(0, m_note_number[0]);
          IGate<0>::note_off(1);
          IGate<0>::note_off(2);
        } else {
          all_note_off();
        }
      }
    }
  }

  INLINE static void set_waveform(uint8_t controller_value) {
    uint8_t waveform;
    if (controller_value < 48) {
      waveform = OSC_WAVEFORM_SAW;
    } else if (controller_value < 80) {
      waveform = OSC_WAVEFORM_ORGAN;
    } else {
      waveform = OSC_WAVEFORM_SQ;
    }

    if (m_waveform != waveform) {
      m_waveform = waveform;
      IOsc<0>::set_waveform(m_waveform);
      if (m_note_number[0] != NOTE_NUMBER_INVALID) {
        IOsc<0>::note_on(0, m_note_number[0]);
      }
      if (m_note_number[1] != NOTE_NUMBER_INVALID) {
        IOsc<0>::note_on(1, m_note_number[1]);
      }
      if (m_note_number[2] != NOTE_NUMBER_INVALID) {
        IOsc<0>::note_on(2, m_note_number[2]);
      }
    }

    if (controller_value < 32) {
      IOsc<0>::set_sub((31 - controller_value) << 2);
    } else if (controller_value >= 96) {
      IOsc<0>::set_sub((controller_value - 96) << 2);
    } else {
      IOsc<0>::set_sub(0);
    }
  }

  INLINE static void note_on(uint8_t note_number) {
    if ((note_number < NOTE_NUMBER_MIN) ||
        (note_number > NOTE_NUMBER_MAX)) {
      return;
    }

    if (m_unison_on) {
      m_note_number[0] = note_number;
      m_note_number[1] = NOTE_NUMBER_INVALID;
      m_note_number[2] = NOTE_NUMBER_INVALID;
      IOsc<0>::note_on(0, note_number);
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
    if (m_unison_on) {
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
    case UNISON:
      set_unison(controller_value);
      break;
    case OSC_WAVEFORM:
      set_waveform(controller_value);
      break;
    case OSC_DETUNE:
      IOsc<0>::set_detune(controller_value);
      break;
    case AMP_EG:
      m_amp_env_amt = controller_value << 1;
      break;
    case FILTER_CUTOFF:
      IFilter<0>::set_cutoff(controller_value);
      break;
    case FILTER_RESO:
      IFilter<0>::set_resonance(controller_value);
      break;
    case FILTER_EG_AMT:
      IFilter<0>::set_env_amt(controller_value);
      break;
    case EG_DECAY:
      IEnvGen<0>::set_decay(controller_value);
      break;
    }
  }

  INLINE static int8_t clock() {
    uint8_t gate_output_array[4];
    IGate<0>::clock();
    gate_output_array[0] = IGate<0>::level<0>();
    gate_output_array[1] = IGate<0>::level<1>();
    gate_output_array[2] = IGate<0>::level<2>();
    gate_output_array[3] = IGate<0>::level<3>();
    int16_t osc_output = IOsc<0>::clock(gate_output_array[0],
                                        gate_output_array[1],
                                        gate_output_array[2]);
    uint8_t env_gen_output = IEnvGen<0>::clock();
    int16_t filter_output = IFilter<0>::clock(osc_output, env_gen_output);
    uint8_t gain_control = high_byte((env_gen_output * m_amp_env_amt) +
                                     ((gate_output_array[3] << 3) *
                                      (254 - m_amp_env_amt)));
    int16_t amp_output = IAmp<0>::clock(filter_output, gain_control);

    // error diffusion
    int16_t output = amp_output + m_output_error;
    m_output_error = low_byte(output);
    return high_sbyte(output);
  }
};

template <uint8_t T> boolean Voice<T>::m_unison_on;
template <uint8_t T> uint8_t Voice<T>::m_waveform;
template <uint8_t T> uint8_t Voice<T>::m_amp_env_amt;
template <uint8_t T> uint8_t Voice<T>::m_note_number[3];
template <uint8_t T> uint8_t Voice<T>::m_output_error;
