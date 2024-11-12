#include <Arduino.h>

unsigned long removeLastThreeDigits(unsigned long hexCode) {
    return hexCode >> 12; // Shift right by 12 bits to remove the last three hex digits
}

int convertToRange(unsigned long value) {
    if (value <= 525) {
        return map(value, 0, 525, 1, 10);
    } else {
        return map(value, 525, 1022, 10, 1);
    }
}

int get_dir(int value) {
    if (value <= 525) {
        return HIGH;
    } else {
        return LOW;
    }
}

void debug_data(int value, int delay, int dir) {
    Serial.print(", value=");
    Serial.print(value);
    Serial.print(", delay=");
    Serial.print(delay);
    Serial.print(", dir=");
    Serial.println(get_dir(value));
}