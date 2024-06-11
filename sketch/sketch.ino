#include "src/button/button.h"
#include "src/pomodoro/pomodoro.h"

#define BAUD 115200
#define LOGGER

#define ONOFF_BUTTON_PIN 9
#define SKIP_BUTTON_PIN 8
#define WORK_LED_PIN 3
#define BREAK_LED_PIN 2
#define LONG_BREAK_INDICATOR_LED_PIN 13

#define BUTTON_DELAY 320
#define WORK_DURATION 3000
#define SMALL_BREAK_DURATION 1000
#define LONG_BREAK_DURATION 3000
#define MAX_WORK_SESSIONS 3

button_o onoff(ONOFF_BUTTON_PIN);
button_o skip_button(SKIP_BUTTON_PIN);
pomodoro_o pomodoro(WORK_DURATION, SMALL_BREAK_DURATION, LONG_BREAK_DURATION,
                    MAX_WORK_SESSIONS);

inline void log_application_state(void);

void setup(void) {
  Serial.begin(BAUD);

  pinMode(ONOFF_BUTTON_PIN, INPUT);
  pinMode(SKIP_BUTTON_PIN, INPUT);
  pinMode(WORK_LED_PIN, OUTPUT);
  pinMode(BREAK_LED_PIN, OUTPUT);
  pinMode(LONG_BREAK_INDICATOR_LED_PIN, OUTPUT);

  onoff.on_click([](void) {
    const unsigned char curr_state = pomodoro.get_state();

    if (curr_state == POMODORO_STATE_OFF)
      pomodoro.start(POMODORO_STATE_WORK);
    else
      pomodoro.stop();

    delay(BUTTON_DELAY);
  });

  skip_button.on_click([](void) {
    const unsigned long timer = millis();
    const unsigned char curr_state = pomodoro.get_state();

    if (curr_state != POMODORO_STATE_OFF)
      pomodoro.skip(timer);

    delay(BUTTON_DELAY);
  });

  pomodoro.on_work([](void) {
    digitalWrite(WORK_LED_PIN, HIGH);
    digitalWrite(BREAK_LED_PIN, LOW);
    digitalWrite(LONG_BREAK_INDICATOR_LED_PIN, LOW);
  });

  pomodoro.on_small_break([](void) {
    digitalWrite(WORK_LED_PIN, LOW);
    digitalWrite(BREAK_LED_PIN, HIGH);
    digitalWrite(LONG_BREAK_INDICATOR_LED_PIN, LOW);
  });

  pomodoro.on_long_break([](void) {
    digitalWrite(WORK_LED_PIN, LOW);
    digitalWrite(BREAK_LED_PIN, HIGH);
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

  log_application_state();
}

inline void log_application_state(void) {
#ifdef LOGGER
  const unsigned long timer = millis();

  const bool is_onoff_pressed = digitalRead(ONOFF_BUTTON_PIN);
  const bool is_skip_button_pressed = digitalRead(SKIP_BUTTON_PIN);

  Serial.print(is_onoff_pressed);
  Serial.print(is_skip_button_pressed);
  Serial.print("\t");

  const unsigned char state = pomodoro.get_state();
  const unsigned short time_left = pomodoro.get_time_left(timer);

  Serial.print(state == POMODORO_STATE_WORK          ? "wor"
               : state == POMODORO_STATE_SMALL_BREAK ? "sbr"
               : state == POMODORO_STATE_LONG_BREAK  ? "lbr"
               : state == POMODORO_STATE_OFF         ? "off"
                                                     : "unk");
  Serial.print("\t");
  Serial.print(state == POMODORO_STATE_WORK          ? WORK_DURATION
               : state == POMODORO_STATE_SMALL_BREAK ? SMALL_BREAK_DURATION
               : state == POMODORO_STATE_LONG_BREAK  ? LONG_BREAK_DURATION
               : state == POMODORO_STATE_OFF         ? 0
                                                     : -1);
  Serial.print("\t");
  Serial.print(time_left);

  Serial.println();
#endif
}
