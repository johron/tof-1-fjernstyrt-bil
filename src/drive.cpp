#include <Arduino.h>
#include <IRremote.hpp>
#include <motor.hpp>
#include <shared.hpp>

const int receiver_pin = 4;
const int drive_dir_pin = 2;
const int drive_step_pin = 3;

const int headlights_com_pin = 5;
const int reverse_lights_com_pin = 6;

int drive_delay = 10;
bool should_drive_step = false;
int dir = HIGH;

void drive_action(int value, int delay) {
    Serial.print("drive_action");
    should_drive_step = true;
    dir = get_dir(value);
    drive_delay = delay;
    debug_data(value, delay, dir);
    digitalWrite(drive_dir_pin, dir);
}

void headlights_action() {
    digitalWrite(headlights_com_pin, HIGH); // jeg har ingen anelse om dette kommmer til å fungere
    delay(100);
    digitalWrite(headlights_com_pin, LOW);
}

void receive() {
    if (IrReceiver.decode()) {
        unsigned long received = bitreverse32Bit(IrReceiver.decodedIRData.decodedRawData);
        unsigned long operation = get_value(received);
        unsigned long value = received & 0xFFF;

        switch (operation) {
            case HEX_MIDDLEX:
                Serial.println("HEX_MIDDLEX");
                should_drive_step = false;
                break;
            case HEX_DRIVE:
                drive_action(value, get_delay(value));
                break;
            case HEX_HEADLIGHTS:
                headlights_action();
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

void reverse_lights() {
    if (dir == LOW) { // gjetter at LOW er rygging
        digitalWrite(reverse_lights_com_pin, HIGH); // jeg har ingen anelse om dette kommmer til å fungere
        delay(100);
        digitalWrite(reverse_lights_com_pin, LOW);
    }
}

void loop() {
    receive();
    drive_step();
    reverse_lights();
}

void setup() {
    Serial.begin(9600);
    digitalWrite(drive_dir_pin, HIGH);
    IrReceiver.begin(receiver_pin, ENABLE_LED_FEEDBACK);
}