#include "flicker.h"

flicker_o::flicker_o(const unsigned long frequency) { _frequency = frequency; }

void flicker_o::refresh(unsigned long timer) {
  if (_state == FLICKER_STATE_OFF and _is_on) {
    _is_on = false;

    return;
  }

  if (_state == FLICKER_STATE_ON and not _is_on) {
    _is_on = true;

    return;
  }

  if (_state != FLICKER_STATE_FLICK)
    return;

  if (timer - _pc > _frequency) {
    _is_on = not _is_on;
    _pc = timer;
  }
}

void flicker_o::turn_on(void) { _state = FLICKER_STATE_ON; }

void flicker_o::turn_off(void) { _state = FLICKER_STATE_OFF; }

void flicker_o::turn_flick(void) { _state = FLICKER_STATE_FLICK; }

bool flicker_o::is_on(void) { return _is_on; }

unsigned long flicker_o::get_frequency(void) { return _frequency; }

unsigned char flicker_o::get_state(void) { return _state; }
