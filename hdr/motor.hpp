#include <Arduino.h>

int get_dir(int value) {
    if (value <= 500) {
        return HIGH;
    } else {
        return LOW;
    }
}

