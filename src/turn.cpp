#include <Arduino.h>
#include <IRremote.hpp>
#include <motor.h>
#include <shared.h>

int receiver_pin = 4;
bool is_locked = true;

int turn_dir_pin = 2;
int turn_step_pin = 3;
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
        unsigned long received = bitreverse32Bit(IrReceiver.decodedIRData.decodedRawData);
        unsigned long operation = removeLastThreeDigits(received);
        unsigned long value = received & 0xFFF;

        switch (operation) {
            case HEX_MIDDLEY:
                Serial.println("HEX_MIDDLEY");
                should_turn_step = false;
                break;
            case HEX_TURN:
                turn_action(value, convertToRange(value));
                break;
            default:
                Serial.println("UNRECOG");
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