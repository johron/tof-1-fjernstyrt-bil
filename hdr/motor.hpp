#include <Arduino.h>

unsigned long get_value(unsigned long hexCode) {
    return hexCode >> 12;
}

int get_dir(int value) {
    if (value <= 500) {
        return HIGH;
    } else {
        return LOW;
    }
}

void debug_data(int value, int dir) {
    Serial.print(", value=");
    Serial.print(value);
    Serial.print(", dir=");
    Serial.println(get_dir(value));
}