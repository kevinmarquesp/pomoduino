#pragma once

#define FLICKER_STATE_OFF 0
#define FLICKER_STATE_ON 1
#define FLICKER_STATE_FLICK 2

class flicker_o {
public:
  flicker_o(const unsigned long);

  void refresh(unsigned long);
  void turn_on(void);
  void turn_off(void);
  void turn_flick(void);

  unsigned char get_state(void);
  unsigned long get_frequency(void);
  bool is_on(void);

private:
  unsigned char _state = FLICKER_STATE_OFF;
  unsigned long _frequency;
  unsigned long _pc = 0;
  bool _is_on = false;
};
