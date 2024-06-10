#define BAUD 115200

#define BLINK_LED 13
#define DELAY 500

bool led_state = LOW;

void setup(void) {
  Serial.begin(BAUD);

  pinMode(BLINK_LED, OUTPUT);
}

void loop(void) {
  Serial.println(led_state ? "Led ON" : "Led OFF");
  digitalWrite(BLINK_LED, led_state);

  led_state = not led_state;

  delay(DELAY);
}
