#include "src/button/button.h"
#include "src/flicker/flicker.h"
#include "src/pomodoro/pomodoro.h"

#define BAUD 115200
#define BUTTON_DELAY 320
#define LOGGER
/* #define DEBUG */

#define ONOFF_BUTTON_PIN 9
#define SKIP_BUTTON_PIN 8
#define WORK_LED_PIN 3
#define BREAK_LED_PIN 2
#define LONG_BREAK_INDICATOR_LED_PIN 13
#define TIMEOUT_BUZZ_PIN 11

#ifndef DEBUG
#define WORK_DURATION 1500000
#define SMALL_BREAK_DURATION 300000
#define LONG_BREAK_DURATION 900000
#define MAX_WORK_SESSIONS 3
#define TIMEOUT_BLINK_ALERT_SPAN 5000
#define TIMEOUT_BUZZ_ALERT_SPAN 1000
#define TIMEOUT_ALERT_FREQUENCY 25
#define TIMEOUT_BUZZ_VOLUME 2
#else
#define WORK_DURATION 5000
#define SMALL_BREAK_DURATION 2000
#define LONG_BREAK_DURATION 3000
#define MAX_WORK_SESSIONS 3
#define TIMEOUT_BLINK_ALERT_SPAN 1000
#define TIMEOUT_BUZZ_ALERT_SPAN 500
#define TIMEOUT_ALERT_FREQUENCY 25
#define TIMEOUT_BUZZ_VOLUME 1
#endif

button_o onoff(ONOFF_BUTTON_PIN);
button_o skip_button(SKIP_BUTTON_PIN);
pomodoro_o pomodoro(WORK_DURATION, SMALL_BREAK_DURATION, LONG_BREAK_DURATION,
                    MAX_WORK_SESSIONS);
flicker_o flicker(TIMEOUT_ALERT_FREQUENCY);

inline void main_loop(void);

void setup(void) {
  Serial.begin(BAUD);

  pinMode(ONOFF_BUTTON_PIN, INPUT);
  pinMode(SKIP_BUTTON_PIN, INPUT);
  pinMode(WORK_LED_PIN, OUTPUT);
  pinMode(BREAK_LED_PIN, OUTPUT);
  pinMode(LONG_BREAK_INDICATOR_LED_PIN, OUTPUT);
  pinMode(TIMEOUT_BUZZ_PIN, OUTPUT);

  onoff.on_click([](void) {
    const unsigned char curr_state = pomodoro.get_state();

    if (curr_state == POMODORO_STATE_OFF) {
      pomodoro.start(POMODORO_STATE_WORK);

    } else {
      pomodoro.stop();
      flicker.turn_off();

      digitalWrite(WORK_LED_PIN, LOW);
      digitalWrite(BREAK_LED_PIN, LOW);
      digitalWrite(LONG_BREAK_INDICATOR_LED_PIN, LOW);
    }

    delay(BUTTON_DELAY);
  });

  skip_button.on_click([](void) {
    const unsigned long timer = millis();
    const unsigned char curr_state = pomodoro.get_state();

    if (curr_state != POMODORO_STATE_OFF)
      pomodoro.skip(timer);

    delay(BUTTON_DELAY);
  });

  pomodoro.on_between([](void) {
    digitalWrite(WORK_LED_PIN, LOW);
    digitalWrite(BREAK_LED_PIN, LOW);
    digitalWrite(LONG_BREAK_INDICATOR_LED_PIN, LOW);
  });

  pomodoro.on_work([](void) { digitalWrite(WORK_LED_PIN, flicker.is_on()); });

  pomodoro.on_small_break(
      [](void) { digitalWrite(BREAK_LED_PIN, flicker.is_on()); });

  pomodoro.on_long_break([](void) {
    digitalWrite(BREAK_LED_PIN, flicker.is_on());
    digitalWrite(LONG_BREAK_INDICATOR_LED_PIN, HIGH);
  });
}

void loop(void) {
  const bool is_onoff_pressed = digitalRead(ONOFF_BUTTON_PIN);
  const bool is_skip_button_pressed = digitalRead(SKIP_BUTTON_PIN);
  const unsigned long timer = millis();

  onoff.refresh(is_onoff_pressed);
  skip_button.refresh(is_skip_button_pressed);
  pomodoro.refresh(timer);
  flicker.refresh(timer);

  main_loop();

#ifdef LOGGER
  const unsigned long curr_duration = pomodoro.get_curr_duration();
  const unsigned char pomodoro_state = pomodoro.get_state();
  const unsigned long time_left = pomodoro.get_time_left(timer);
  const unsigned long flicker_frequency = flicker.get_frequency();
  const unsigned char flicker_state = flicker.get_state();
  const bool is_flicker_on = flicker.is_on();

  char curr_duration_buff[9];
  char time_left_buff[9];

  sprintf(curr_duration_buff, "%0lX", curr_duration);
  sprintf(time_left_buff, "%0lX", time_left);

  Serial.print(is_onoff_pressed);
  Serial.print(is_skip_button_pressed);
  Serial.print("\t");
  Serial.print(flicker_state == FLICKER_STATE_OFF     ? "off"
               : flicker_state == FLICKER_STATE_ON    ? "onn"
               : flicker_state == FLICKER_STATE_FLICK ? "flk"
                                                      : "unk");
  Serial.print("\t");
  Serial.print(flicker_frequency);
  Serial.print("\t");
  Serial.print(is_flicker_on);
  Serial.print("\t");
  Serial.print(pomodoro_state == POMODORO_STATE_WORK          ? "wor"
               : pomodoro_state == POMODORO_STATE_SMALL_BREAK ? "sbr"
               : pomodoro_state == POMODORO_STATE_LONG_BREAK  ? "lbr"
               : pomodoro_state == POMODORO_STATE_OFF         ? "off"
                                                              : "unk");
  Serial.print("\t");
  Serial.print(curr_duration_buff);
  Serial.print("\t");
  Serial.print(time_left_buff);
  Serial.println();
#endif
}

inline void main_loop(void) {
  const unsigned char pomodoro_state = pomodoro.get_state();

  if (pomodoro_state == POMODORO_STATE_OFF)
    return;

  const unsigned long timer = millis();
  const unsigned long time_left = pomodoro.get_time_left(timer);
  const unsigned char flicker_state = flicker.get_state();
  const bool is_flicker_on = flicker.is_on();

  if (time_left < TIMEOUT_BLINK_ALERT_SPAN)
    flicker.turn_flick();
  else
    flicker.turn_on();

  if (flicker_state == FLICKER_STATE_FLICK and
      time_left < TIMEOUT_BUZZ_ALERT_SPAN)
    analogWrite(TIMEOUT_BUZZ_PIN, is_flicker_on ? TIMEOUT_BUZZ_VOLUME : 0);
  else
    digitalWrite(TIMEOUT_BUZZ_PIN, LOW);
}
