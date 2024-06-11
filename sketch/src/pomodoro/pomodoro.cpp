#include "pomodoro.h"

pomodoro_o::pomodoro_o(const unsigned long work_duration,
                       const unsigned long small_break_duration,
                       const unsigned long long_break_duration,
                       const unsigned char max_work_sessions) {
  _work_duration = work_duration;
  _small_break_duration = small_break_duration;
  _long_break_duration = long_break_duration;
  _max_work_sessions = max_work_sessions;
}

void pomodoro_o::refresh(unsigned long timer) {
  if (_state == POMODORO_STATE_OFF) {
    _pc = timer;

    return;
  }

  _curr_duration = _state == POMODORO_STATE_WORK ? _work_duration
                   : _state == POMODORO_STATE_SMALL_BREAK
                       ? _small_break_duration
                       : _long_break_duration;

  if (timer - _pc > _curr_duration) {
    if (_exec_between_routine != nullptr)
      _exec_between_routine();

    if (_state == POMODORO_STATE_LONG_BREAK)
      _work_counter = 0;
    else if (_state == POMODORO_STATE_WORK)
      ++_work_counter;

    _state = this->get_next_state();
    _pc = timer;
  }

  switch (_state) {
  case POMODORO_STATE_WORK:
    if (_exec_work_routine != nullptr)
      _exec_work_routine();
    break;

  case POMODORO_STATE_SMALL_BREAK:
    if (_exec_small_break_routine != nullptr)
      _exec_small_break_routine();
    break;

  case POMODORO_STATE_LONG_BREAK:
    if (_exec_long_break_routine != nullptr)
      _exec_long_break_routine();
    break;
  }
}

void pomodoro_o::stop(void) { _state = POMODORO_STATE_OFF; }

void pomodoro_o::start(const unsigned char new_state) { _state = new_state; }

void pomodoro_o::on_work(void (*u_func)(void)) { _exec_work_routine = u_func; }

void pomodoro_o::on_small_break(void (*u_func)(void)) {
  _exec_small_break_routine = u_func;
}
void pomodoro_o::on_long_break(void (*u_func)(void)) {
  _exec_long_break_routine = u_func;
}
void pomodoro_o::on_between(void (*u_func)(void)) {
  _exec_between_routine = u_func;
}

unsigned char pomodoro_o::get_state(void) { return _state; }

unsigned char pomodoro_o::get_next_state(void) {
  return _is_in_break()          ? POMODORO_STATE_WORK
         : _is_long_break_time() ? POMODORO_STATE_LONG_BREAK
                                 : POMODORO_STATE_SMALL_BREAK;
}

bool pomodoro_o::_is_in_break(void) {
  return _state == POMODORO_STATE_SMALL_BREAK ||
         _state == POMODORO_STATE_LONG_BREAK;
}

bool pomodoro_o::_is_long_break_time(void) {
  return _work_counter >= _max_work_sessions;
}

unsigned long pomodoro_o::get_time_left(unsigned long timer) {
  return _curr_duration - (timer - _pc);
}
