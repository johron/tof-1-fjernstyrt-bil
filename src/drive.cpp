#include <Arduino.h>
#include <IRremote.hpp>
#include <motor.h>
#include <shared.h>

const int receiver_pin = 4;
const int drive_dir_pin = 2;
const int drive_step_pin = 3;
const int lights_com_pin = 5;

int drive_delay = 10;
bool should_drive_step = false;

void drive_action(int value, int delay) {
    Serial.print("drive_action");
    should_drive_step = true;
    int dir = get_dir(value);
    debug_data(value, delay, dir);
    drive_delay = delay;
    digitalWrite(drive_dir_pin, dir);
}

void lights_action() {
    digitalWrite(lights_com_pin, HIGH); // jeg har ingen anelse om dette kommmer til å fungere
    delay(100);
    digitalWrite(lights_com_pin, LOW);
}

void receive() {
    if (IrReceiver.decode()) {
        unsigned long received = bitreverse32Bit(IrReceiver.decodedIRData.decodedRawData);
        unsigned long operation = removeLastThreeDigits(received);
        unsigned long value = received & 0xFFF;

        switch (operation) {
            case HEX_MIDDLEX:
                Serial.println("HEX_MIDDLEX");
                should_drive_step = false;
                break;
            case HEX_DRIVE:
                drive_action(value, convertToRange(value));
                break;
            case HEX_LIGHTS:
                lights_action();
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

void loop() {
    receive();
    drive_step();
}

void setup() {
    Serial.begin(9600);
    digitalWrite(drive_dir_pin, HIGH);
    IrReceiver.begin(receiver_pin, ENABLE_LED_FEEDBACK);
}