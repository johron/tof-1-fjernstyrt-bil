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