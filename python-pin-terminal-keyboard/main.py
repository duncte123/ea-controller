# WARNING: here be AI SLOP
# I could not be arsed to write this python script myself, so slop it is.
# Seems to work fine tho and that is all I wanted.

import serial
import keyboard
import logging

# -----------------------------------------------------------------------------
# Configuration
# -----------------------------------------------------------------------------

# SERIAL_PORT = "COM3"          # Windows example
SERIAL_PORT = "/dev/ttyACM0"  # Linux example

BAUD_RATE = 115200

# Mapping from serial button codes to keyboard keys.
# See https://github.com/boppreh/keyboard#api
KEY_MAP = {
    "0": "0",
    "1": "1",
    # "2": "2",
    "2": "w",
    "3": "3",
    # "4": "4",
    # "5": "5",
    # "6": "6",
    "4": "a",
    "5": "s",
    "6": "d",
    "7": "7",
    "8": "8",
    "9": "9",
    ".": ".",

    # "Ok": "enter",
    "Ok": "space",
    "Stop": "esc",
    "Corr": "backspace",
    "?": "shift",
    "M": "m",

    "F1": "f1",
    "F2": "f2",
    "F3": "f3",
}

# -----------------------------------------------------------------------------

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s %(levelname)s: %(message)s",
)

# Track currently pressed keys so we don't press twice.
pressed = set()


def handle_event(code: str, state: str):
    """Handle one serial button event."""

    key = KEY_MAP.get(code)

    if key is None:
        logging.warning("Unknown button code: %s", code)
        return

    state = state.upper()

    if state == "DOWN":
        if key not in pressed:
            keyboard.press(key)
            pressed.add(key)
            logging.info("%s -> PRESS %s", code, key)

    elif state == "UP":
        if key in pressed:
            keyboard.release(key)
            pressed.remove(key)
            logging.info("%s -> RELEASE %s", code, key)

    else:
        logging.warning("Unknown state: %s", state)


def main():
    logging.info(
        "Opening %s at %d baud...",
        SERIAL_PORT,
        BAUD_RATE,
    )

    with serial.Serial(
        port=SERIAL_PORT,
        baudrate=BAUD_RATE,
        timeout=1,
    ) as ser:

        logging.info("Listening...")

        while True:
            try:
                line = ser.readline().decode("utf-8", errors="replace").strip()

                if not line:
                    continue

                parts = line.split()

                if len(parts) != 2:
                    logging.warning("Malformed line: %r", line)
                    continue

                code, state = parts
                handle_event(code, state)

            except KeyboardInterrupt:
                break
            except Exception:
                logging.exception("Error processing serial data")

    # Make sure no keys remain held when exiting.
    for key in list(pressed):
        keyboard.release(key)

    logging.info("Exited cleanly.")


if __name__ == "__main__":
    main()