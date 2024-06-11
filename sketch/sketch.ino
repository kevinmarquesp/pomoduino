#include "src/button/button.h"
#include "src/pomodoro/pomodoro.h"

#define BAUD 115200
#define ONOFF_PIN 9

#define BUTTON_DELAY 320
#define WORK_DURATION 3000
#define SMALL_BREAK_DURATION 1000
#define LONG_BREAK_DURATION 3000
#define MAX_WORK_SESSIONS 3

button_o onoff(ONOFF_PIN);
pomodoro_o pomodoro(WORK_DURATION, SMALL_BREAK_DURATION, LONG_BREAK_DURATION,
                    MAX_WORK_SESSIONS);

bool is_onoff_pressed;
unsigned long timer = 0;

void setup(void) {
  Serial.begin(BAUD);

  pinMode(ONOFF_PIN, INPUT);

  onoff.on_click([](void) {
    const unsigned char curr_state = pomodoro.get_state();

    Serial.println(curr_state);

    if (curr_state == POMODORO_STATE_OFF)
      pomodoro.start(POMODORO_STATE_WORK);
    else
      pomodoro.stop();

    delay(BUTTON_DELAY);
  });

  pomodoro.on_work([](void) {
    const unsigned short time_left = pomodoro.get_time_left(timer);

    Serial.print("Working: ");
    Serial.println(time_left);
  });

  pomodoro.on_small_break([](void) {
    const unsigned short time_left = pomodoro.get_time_left(timer);

    Serial.print("Small break: ");
    Serial.println(time_left);
  });

  pomodoro.on_long_break([](void) {
    const unsigned short time_left = pomodoro.get_time_left(timer);

    Serial.print("Long break: ");
    Serial.println(time_left);
  });
}

void loop(void) {
  is_onoff_pressed = digitalRead(ONOFF_PIN);
  timer = millis();

  onoff.refresh(is_onoff_pressed);
  pomodoro.refresh(timer);
}
