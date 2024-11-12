#include <Arduino.h>
#include <CircularBuffer.hpp>
#include <IRremote.hpp>
#include <OneButton.h>
#include <shared.h>

int vrx_pin = A0;
int vry_pin = A1;

int ir_led_pin = 2;
int joystick_btn_pin = 3;

bool sent_middlex_last = false;
bool sent_middley_last = false;

CircularBuffer<long, 15> sentLast;

OneButton JoystickButton(joystick_btn_pin, true, true);

void sendIR(unsigned long hex) {
    unsigned long newhex = hex;
    if ((newhex & 0xFFF00000) == 0) { // Check if the hex code has less than 32 bits
        newhex = (newhex << 12) | 0xFFF; // Append FFF to the end
    }
    
    bool allSame = true;
    for (int i = 0; i < sentLast.size(); i++) {
        if (sentLast[i] != newhex) {
            allSame = false;
            break;
        }
    }
    if (allSame && sentLast.size() > 0) {
        return;
    }

    sentLast.push(newhex);
    IrSender.sendNECMSB(newhex, 32);
    delay(10);
}

void joystick() {
    int xValue = analogRead(vrx_pin); 
    int yValue = analogRead(vry_pin);

    if (xValue < 500 || xValue > 550) { // Speed
        sent_middlex_last = false;
        unsigned long command = (HEX_DRIVE << 12) | (xValue & 0xFFF);
        sendIR(command);
    }

    if (yValue < 500 || yValue > 550) { // Turn
        sent_middley_last = false;
        unsigned long command = (HEX_TURN << 12) | (yValue & 0xFFF);
        sendIR(command);
    }

    if (xValue >= 500 && xValue <= 550) {
        sent_middlex_last = true;
        sendIR(HEX_MIDDLEX);
    }

    if (yValue >= 500 && yValue <= 550) {
        sent_middley_last = true;
        sendIR(HEX_MIDDLEY);
    }
    
    delay(10);
}

static void clickJoystickButton() {
    //sendIR(HEX_LOCK);
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