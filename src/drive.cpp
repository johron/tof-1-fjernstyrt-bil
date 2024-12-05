#include <Arduino.h>
#include <IRremote.hpp>

#include "shared.hpp"
#include "motor.hpp"

const int drive_step_pin = 2;
const int drive_dir_pin = 3;
const int receiver_pin = 4;

bool should_drive_step = false;

void drive_action(int value) {
    Serial.print("drive_action");
    should_drive_step = true;
    int dir = get_dir(value);
    debug_data(value, dir);
    digitalWrite(drive_dir_pin, dir);
}

void receive() {
    if (IrReceiver.decode()) {
        unsigned long received = bitreverse32Bit(IrReceiver.decodedIRData.decodedRawData);
        unsigned long operation = get_value(received);
        unsigned long value = received & 0xFFF;

        if (operation == HEX_DRIVE) {
            drive_action(value); 
        }

        switch (operation) {
            case HEX_MIDDLE:
                if (value == 0x000) {
                    Serial.println("HEX_MIDDLEX");
                    should_drive_step = false;
                }
                break;
            case HEX_DRIVE:
                drive_action(value, get_delay(value));
                break;
            default:
                Serial.print(value);
                Serial.println(" - UNRECOG");
                break;
        }
        
        IrReceiver.resume();
    }
}

void drive_step() {
    if (should_drive_step) {
        digitalWrite(drive_step_pin, HIGH);
        delay(drive_delay);
        digitalWrite(drive_step_pin, LOW); // hugin sier at jeg bør prøve å snu disse om for å se om det går raskere.
    }
}

void test() {
    digitalWrite(drive_step_pin, HIGH);
    delay(1);
    digitalWrite(drive_step_pin, LOW);
}

void loop() {
    //test();
    static unsigned long last_receive_time = 0;
    unsigned long current_time = millis();
    if (current_time - last_receive_time >= 1000) {
        receive();
        last_receive_time = current_time;
    }
    drive_step();
}

void setup() {
    Serial.begin(9600);
    digitalWrite(drive_dir_pin, HIGH);
    IrReceiver.begin(receiver_pin, ENABLE_LED_FEEDBACK);
}