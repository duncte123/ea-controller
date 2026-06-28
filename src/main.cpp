#include <Arduino.h>

#define LED_PIN 2
#define KEY_UP LOW
#define KEY_DOWN HIGH

String keyboard[4][4] = {
    {"1", "2", "3", "Stop"},
    {"4", "5", "6", "Corr"},
    {"7", "8", "9", "?"},
    {"M", "0", " ", "Ok"},
};

// Row pins == Outputs
int rowPins[4] = {
    21, // Row 1
    22, // Row 2
    17, // Row 3
    16  // Row 4
};
// Colum pins == Inputs
int colPins[4] = {
    23, // Col 1
    19, // Col 2
    18, // Col 3
    26  // Col 4
};

void setupRowAndColPins() {
    for (int i = 0; i < 4; i++) {
        pinMode(rowPins[i], OUTPUT);
        pinMode(colPins[i], INPUT_PULLDOWN);
    }
}

void initRowPins() {
    for (const int rowPin : rowPins) {
        digitalWrite(rowPin, LOW);
    }
}

void setLeds(const int state) {
    digitalWrite(LED_PIN, state);
    digitalWrite(LED_BUILTIN, state);
}

void writeRow(const int row, const int state) {
    digitalWrite(rowPins[row], state);
}

int readCol(const int col) {
    return digitalRead(colPins[col]);
}

int scan(const int row, const int col) {
    int keyState = LOW;

    writeRow(row, HIGH);

    keyState = readCol(col);

    writeRow(row, LOW);

    return keyState;
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    setLeds(LOW);

    setupRowAndColPins();
    initRowPins();
}

int ledState = LOW;

// void loop() {
//     if (ledState == LOW) {
//         Serial.println("Light state is HIGH");
//         ledState = HIGH;
//     } else {
//         Serial.println("Light state is LOW");
//         ledState = LOW;
//     }
//
//
//     digitalWrite(LED_PIN, ledState);
//     digitalWrite(LED_BUILTIN, ledState);
//
//     delay(1000);
// }

String lastKeyPressed = "";
ulong lastMillis = 0;

// TODO: debounce
void loop() {
    const auto currentMillis = millis();

    if (currentMillis - lastMillis > 1000) {
        lastMillis = currentMillis;

        if (ledState == LOW) {
            ledState = HIGH;
        } else {
            ledState = LOW;
        }

        setLeds(ledState);
    }

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            const int keyState = scan(row, col);
            const String key = keyboard[row][col];

            if (keyState == KEY_DOWN && lastKeyPressed != key) {
                // Serial.print("Key pressed: \"");
                Serial.print(key);
                // Serial.println("\"");

                lastKeyPressed = key;
            } else if (keyState == KEY_UP && lastKeyPressed == key) {
                lastKeyPressed = "";
            }
        }
    }
}
