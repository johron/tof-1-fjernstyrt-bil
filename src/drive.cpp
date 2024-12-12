#include <Arduino.h>
#include <IRremote.hpp>

#include "../hdr/shared.hpp"
#include "../hdr/motor.hpp"

const int step_pin = 2;
const int dir_pin = 3;
const int receiver_pin = 4;

bool should_step = false;
int step_delay = 10;

static unsigned long last_time = 0;

void setup_step(int value) {
    should_step = true;
    step_delay = get_delay(value);
    Serial.print("drive_action");
    debug_data(value, get_dir(value));
    digitalWrite(dir_pin, get_dir(value));
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
        } else {
            Serial.print("unrecog, value=");
            Serial.println(value);
        }

        IrReceiver.resume();
    }
}

void step() {
    if (should_step) {
        digitalWrite(step_pin, HIGH);
        delay(step_delay);
        digitalWrite(step_pin, LOW);
    }
}

void test() {
    digitalWrite(step_pin, HIGH);
    delay(1);
    digitalWrite(step_pin, LOW);
}

void loop() {
    //unsigned long current_time = millis();
    //if (current_time - last_time >= 500) {
    //    receive();
    //    last_time = current_time;
    //}
    test();
    //step();
}

void setup() {
    Serial.begin(9600);
    digitalWrite(dir_pin, HIGH);
    IrReceiver.begin(receiver_pin, ENABLE_LED_FEEDBACK);
}