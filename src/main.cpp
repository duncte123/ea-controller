#include <Arduino.h>
#include <map>

#define LED_PIN 2
#define KEY_UP LOW
#define KEY_DOWN HIGH

// Function pin common (black)
#define FUNCTION_COMMON 25
// Function 1 (brown)
#define FUNCTION_1 5
// Function 2 (red)
#define FUNCTION_2 33
// Function 3 (orange)
#define FUNCTION_3 32

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
    {"F1", false},
    {"F2", false},
    {"F3", false},
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

    pinMode(FUNCTION_COMMON, OUTPUT);
    pinMode(FUNCTION_1, INPUT_PULLDOWN);
    pinMode(FUNCTION_2, INPUT_PULLDOWN);
    pinMode(FUNCTION_3, INPUT_PULLDOWN);
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

static void checkFunctionKeyPress(const int espPin, const String& key) {
    // digitalWrite(FUNCTION_COMMON, HIGH);
    const auto keyState = digitalRead(espPin);
    // digitalWrite(FUNCTION_COMMON, LOW);

    if (keyState == KEY_DOWN && !keyStates[key]) {
        Serial.print(key);
        Serial.print(" ");
        Serial.println("DOWN");

        keyStates[key] = true;
    } else if (keyState == KEY_UP && keyStates[key]) {
        keyStates[key] = false;
        Serial.print(key);
        Serial.print(" ");
        Serial.println("UP");
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    setLeds(LOW);

    setupRowAndColPins();
    initRowPins();
}

// static int ledState = LOW;

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

    // TODO: optimize and only set the row high once per row
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
            } else if (keyState == KEY_UP && keyStates[key]) {
                keyStates[key] = false;
                Serial.print(key);
                Serial.print(" ");
                Serial.println("UP");
            }
        }
    }

    // Manual check for function rows
    digitalWrite(FUNCTION_COMMON, HIGH);
    checkFunctionKeyPress(FUNCTION_1, "F1");
    checkFunctionKeyPress(FUNCTION_2, "F2");
    checkFunctionKeyPress(FUNCTION_3, "F3");

    digitalWrite(FUNCTION_COMMON, LOW);

    bool anyKeyDown = false;
    for (const auto& [_, value] : keyStates) {
        if (value) {
            anyKeyDown = true;
            break;
        }
    }

    setLeds(anyKeyDown ? HIGH : LOW);
}
