#include <Arduino.h>
#include "OneButton.h"
#include <IRremote.hpp>

int vrx_pin = A0;
int vry_pin = A1;

int ir_led_pin = 2;
int joystick_btn_pin = 3;

bool sent_middle_last = false;

enum Commands {
    HEX_MIDDLE = 0xBE489,
    HEX_BRAKE = 0xAB34D,
    HEX_DRIVE = 0xE4C24,
    HEX_TURN = 0x2CD5A,
};

OneButton JoystickButton(joystick_btn_pin, true, true);

void sendIR(unsigned long hex) {
    unsigned long newhex = hex;
    if ((newhex & 0xFFF00000) == 0) { // Check if the hex code has less than 32 bits
        newhex = (newhex << 12) | 0xFFF; // Append FFF to the end
    }
    IrSender.sendNECMSB(newhex, 32);
    delay(10);
}

void joystick() {
    int xValue = analogRead(vrx_pin); 
    int yValue = analogRead(vry_pin);
    
    if (xValue < 500 || xValue > 550) { // Speed
        sent_middle_last = false;
        unsigned long command = (HEX_DRIVE << 12) | (xValue & 0xFFF);
        sendIR(command);
    }

    if (yValue < 500 || yValue > 550) { // Turn
        sent_middle_last = false;
        unsigned long command = (HEX_TURN << 12) | (yValue & 0xFFF);
        sendIR(command);
    }

    if (!sent_middle_last) {
        sent_middle_last = true;
        if (xValue >= 500 && xValue <= 550) {
            unsigned long command = (HEX_MIDDLE << 12) | (100 & 0xFFF);
            sendIR(command);
        } else if (yValue >= 500 && yValue <= 550) {
            unsigned long command = (HEX_MIDDLE << 12) | (200 & 0xFFF);
            sendIR(command);
        }
    } else {
        sent_middle_last = false;
    }
    
    delay(10);
}

static void clickJoystickButton() {
    sendIR(HEX_BRAKE);
}

void buttons() {
    JoystickButton.attachClick(clickJoystickButton);
    JoystickButton.tick();
}

void loop() {
    joystick();
    buttons();
}

void setup() {
    Serial.begin(9600);
    IrSender.begin(ir_led_pin);
}