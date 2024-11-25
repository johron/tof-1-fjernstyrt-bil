#include <Arduino.h>

unsigned long get_value(unsigned long hexCode) {
    return hexCode >> 12;
}

int get_delay(unsigned long value) {
    if (value <= 525) {
        return map(value, 0, 511, 1, 10);
    } else {
        return map(value, 511, 1022, 10, 1);
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