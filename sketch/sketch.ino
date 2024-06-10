#include "src/button/button.h"

#define BAUD 115200

#define ONOFF_PIN 9 // Button pin #1.
#define BUTTON_DELAY 120

button_o onoff(ONOFF_PIN);

bool is_onoff_pressed;

void setup(void) {
  Serial.begin(BAUD);

	pinMode(ONOFF_PIN, INPUT);

	onoff.on_click([](void){
		delay(BUTTON_DELAY);

		Serial.println("Hello world!");
	});
}

void loop(void) {
	is_onoff_pressed = digitalRead(ONOFF_PIN);

	onoff.refresh(is_onoff_pressed);
}
