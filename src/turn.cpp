#include <Arduino.h>
#include <IRremote.hpp>
#include <motor.hpp>
#include <shared.hpp>

const int receiver_pin = 4;
const int turn_dir_pin = 2;
const int turn_step_pin = 3;

int turn_delay = 10;
bool should_turn_step = false;

void turn_action(int value, int delay) {
    Serial.print("turn_action");
    should_turn_step = true;
    int dir = get_dir(value);
    debug_data(value, delay, dir);
    turn_delay = delay;
    digitalWrite(turn_dir_pin, dir);
}

void receive() {
    if (IrReceiver.decode()) {
        unsigned long received = bitreverse32Bit(IrReceiver.decodedIRData.decodedRawData); /* The number is received as LSB, but have to reverse it to MSB
                                                                                              https://techdocs.altium.com/display/FPGA/NEC+Infrared+Transmission+Protocol*/
        unsigned long operation = get_value(received);
        unsigned long value = received & 0xFFF; // Remove the instruction hex number so the only number left is the for example joystick x value

        switch (operation) {
            case HEX_MIDDLE:
                if (value == 0x000) {
                    Serial.println("HEX_MIDDLEY");
                    should_turn_step = false;
                }
                break;
            case HEX_TURN:
                turn_action(value, get_delay(value));
                break;
            default:
                Serial.print(value);
                Serial.print(" - ");
                Serial.print(operation);
                Serial.println(" - UNRECOG");
                break;
        }
        
        IrReceiver.resume();
    }
}

void turn_step() {
    if (should_turn_step) {
        digitalWrite(turn_step_pin, HIGH);
        delay(turn_delay);
        digitalWrite(turn_step_pin, LOW);
    }
}

void loop() {
    receive();
    turn_step();
}

void setup() {
    Serial.begin(9600);
    digitalWrite(turn_dir_pin, HIGH);
    IrReceiver.begin(receiver_pin, ENABLE_LED_FEEDBACK);
}