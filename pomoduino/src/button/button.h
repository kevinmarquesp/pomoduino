#pragma once

#define REFRESH_DELAY 250

class button_o {
public:
  button_o(const char);

  void on_click(void (*)(void));
  void refresh(bool);

private:
  char _pin;
  bool _is_locked = false;

  void (*_exec_on_click)(void) = nullptr;
};
