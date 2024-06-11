#pragma once

#define POMODORO_STATE_OFF 0
#define POMODORO_STATE_WORK 1
#define POMODORO_STATE_SMALL_BREAK 2
#define POMODORO_STATE_LONG_BREAK 3

class pomodoro_o {
public:
  pomodoro_o(const unsigned long, const unsigned long, const unsigned long,
             const unsigned char);

  void refresh(unsigned long);
  void stop(void);
  void start(const unsigned char);

  void on_work(void (*)(void));
  void on_small_break(void (*)(void));
  void on_long_break(void (*)(void));
  void on_between(void (*)(void));

  unsigned char get_state(void);
  unsigned char get_next_state(void);
  unsigned long get_time_left(unsigned long);

private:
  unsigned char _state = POMODORO_STATE_WORK;
  unsigned char _work_counter = 0;
  unsigned char _max_work_sessions = 0;
  unsigned long _pc = 0;
  unsigned long _work_duration = 0;
  unsigned long _small_break_duration = 0;
  unsigned long _long_break_duration = 0;
  unsigned long _curr_duration = 0;

  void (*_exec_work_routine)(void) = nullptr;
  void (*_exec_small_break_routine)(void) = nullptr;
  void (*_exec_long_break_routine)(void) = nullptr;
  void (*_exec_between_routine)(void) = nullptr;

  bool _is_in_break(void);
  bool _is_long_break_time(void);
};
