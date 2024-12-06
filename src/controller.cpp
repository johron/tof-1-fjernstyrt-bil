#include <Arduino.h>
#include <IRremote.hpp>
#include <OneButton.h>

#include "../hdr/shared.hpp"

const int vry_pin = A0;
const int vrx_pin = A1;
const int ir_led_pin = 2;
const int joystick_btn_pin = 3;

unsigned long lastCommands[2];

// Function to send IR command
void sendIR(unsigned long hex) {
    unsigned long newhex = hex;

    // Check if the hex number is eight digits long
    if ((newhex & 0xFFF00000) == 0) {
        newhex = (newhex << 12) | 0xFFF; /* Add three hexadecimal digits at the end FFF (dec:4095)
                                            so the number is eight digits long */
    }

    IrSender.sendNECMSB(newhex, 32); /* 32 because it is a eight digit hexadecimal
                                        number which is 32 bits 8*16=32 */
    delay(10);
}

void joystick() {
    int xValue = analogRead(vrx_pin);
    int yValue = analogRead(vry_pin);
    unsigned long commands[2];

    if (xValue < 450) {
        commands[0] = (HEX_DRIVE << 12) | 0x3FF;
    } else if (xValue > 550) {
        commands[0] = (HEX_DRIVE << 12) | 0x000;
    }

    if (yValue < 450) {
        commands[1] = (HEX_TURN << 12) | 0x3FF;
    } else if (yValue > 550) {
        commands[1] = (HEX_TURN << 12) | 0x000;
    }

    for (int i = 0; i < 2; i++) {
        if (commands[i] != 0) {
            if (commands[i] != lastCommands[i]) break;
            sendIR(commands[i]);
        }
        lastCommands[i] = commands[i];
    }

    delay(10);
}

void loop() {
    joystick();
}

void setup() {
    Serial.begin(9600);
    IrSender.begin(ir_led_pin);
}