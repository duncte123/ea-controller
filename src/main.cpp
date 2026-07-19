#include <Arduino.h>
#include <map>

#define LED_PIN 2
#define KEY_UP LOW
#define KEY_DOWN HIGH

static String keyboard[4][4] = {
    {"1", "2", "3", "Stop"},
    {"4", "5", "6", "Corr"},
    {"7", "8", "9", "?"},
    {"M", "0", ".", "Ok"},
};

static std::map<String, bool> keyStates = {
    {"1", false},
    {"2", false},
    {"3", false},
    {"4", false},
    {"5", false},
    {"6", false},
    {"7", false},
    {"8", false},
    {"9", false},
    {"0", false},
    {"M", false},
    {".", false},
    {"Stop", false},
    {"Corr", false},
    {"?", false},
    {"Ok", false},
};

// Row pins == Outputs
static int rowPins[4] = {
    21, // Row 1 (blue)
    22, // Row 2 (green)
    17, // Row 3 (purple)
    16  // Row 4 (gray)
};
// Colum pins == Inputs
static int colPins[4] = {
    23, // Col 1 (yellow)
    19, // Col 2 (orange)
    18, // Col 3 (red)
    26  // Col 4 (brown)
};

static void setupRowAndColPins() {
    for (int i = 0; i < 4; i++) {
        pinMode(rowPins[i], OUTPUT);
        pinMode(colPins[i], INPUT_PULLDOWN);
    }
}

static void initRowPins() {
    for (const int rowPin : rowPins) {
        digitalWrite(rowPin, LOW);
    }
}

static void setLeds(const int state) {
    digitalWrite(LED_PIN, state);
    digitalWrite(LED_BUILTIN, state);
}

static void writeRow(const int row, const int state) {
    digitalWrite(rowPins[row], state);
}

static int readCol(const int col) {
    return digitalRead(colPins[col]);
}

static int scan(const int row, const int col) {
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

static int ledState = LOW;

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

static String lastKeyPressed = "";
static ulong lastMillis = 0;

// TODO: debounce
void loop() {
    /*const auto currentMillis = millis();

    if (currentMillis - lastMillis > 1000) {
        lastMillis = currentMillis;

        if (ledState == LOW) {
            ledState = HIGH;
        } else {
            ledState = LOW;
        }

        setLeds(ledState);
    }*/

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            const int keyState = scan(row, col);
            const String key = keyboard[row][col];

            if (keyState == KEY_DOWN && !keyStates[key]) {
                // Serial.print("Key pressed: \"");
                Serial.print(key);
                Serial.print(" ");
                Serial.println("DOWN");
                // Serial.println("\"");

                keyStates[key] = true;

                lastKeyPressed = key;
            } else if (keyState == KEY_UP && keyStates[key]) {
                lastKeyPressed = "";
                keyStates[key] = false;
                Serial.print(key);
                Serial.print(" ");
                Serial.println("UP");
            }
        }
    }

    bool anyKeyDown = false;
    for (const auto& [_, value] : keyStates) {
        if (value) {
            anyKeyDown = true;
            break;
        }
    }

    setLeds(anyKeyDown ? HIGH : LOW);
}
