#include <Arduino.h>
#include <IRremote.hpp>

int receiver_pin = 4;
bool is_locked = true;

int turn_dir_pin = 6;
int turn_step_pin = 5;
int turn_delay = 10;
bool should_turn_step = false;

enum Commands {
    HEX_MIDDLEX = 0xBE489,
    HEX_MIDDLEY = 0x4A4F5,
    HEX_DRIVE = 0xE4C24,
    HEX_TURN = 0x2CD5A,
};

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