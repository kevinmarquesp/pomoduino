ARDUINO_CLI = arduino-cli
SKETCH = sketch

PORT = /dev/ttyUSB0
BOARD = arduino:avr:uno
BAUD = 115200

.PHONY: default
default: upload serial

.PHONY: compile
compile:
	$(ARDUINO_CLI) compile --port $(PORT) --fqbn $(BOARD) $(SKETCH)

.PHONY: upload
upload: compile
	$(ARDUINO_CLI) upload --port $(PORT) --fqbn $(BOARD) $(SKETCH)

.PHONY: serial
serial:
	$(ARDUINO_CLI) monitor --port $(PORT) --config "baudrate=$(BAUD)"
