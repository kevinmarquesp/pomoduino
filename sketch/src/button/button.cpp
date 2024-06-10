#include "button.h"

button_o::button_o(char pin) { _pin = pin; }

void button_o::on_click(void (*u_func)(void)) { _exec = u_func; }

void button_o::refresh(bool is_pressed) {
  if (is_pressed and not _is_locked) {
    _is_locked = true;

    _exec();
  }

  if (not is_pressed and _is_locked)
    _is_locked = false;
}
