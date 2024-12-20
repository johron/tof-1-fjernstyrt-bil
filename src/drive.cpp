#include <Arduino.h>
#include <IRremote.hpp>

#include "../hdr/shared.hpp"
#include "../hdr/motor.hpp"

const int step_pin = 2;
const int dir_pin = 3;
const int receiver_pin = 4;

bool should_step = false;
static unsigned long last_time = 0;

void setup_step(int value) {
    should_step = true;
    int dir = get_dir(value);
    digitalWrite(dir_pin, dir);
}

void receive() {
    if (IrReceiver.decode()) {
        unsigned long received = bitreverse32Bit(IrReceiver.decodedIRData.decodedRawData);
        unsigned long operation = get_value(received);
        unsigned long value = received & 0xFFF;

        if (operation == HEX_DRIVE) {
            setup_step(value);
        } else if (operation == HEX_STOP_D) {
            should_step = false;
        }

        IrReceiver.resume();
    }
}

void step() {
    if (should_step) {
        digitalWrite(step_pin, HIGH);
        delay(1);
        digitalWrite(step_pin, LOW);
    }
}

void loop() {
    unsigned long current_time = millis();
    if (current_time - last_time >= 300) {
        for (int i = 0; i < 10; i++) {
            receive();
        }
        receive();
        last_time = current_time;
    }
    step();
}

void setup() {
    digitalWrite(dir_pin, HIGH);
    IrReceiver.begin(receiver_pin, ENABLE_LED_FEEDBACK);
}