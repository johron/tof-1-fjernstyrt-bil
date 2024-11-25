#include <Arduino.h>
#include <IRremote.hpp>
#include <OneButton.h>
#include <shared.h>

int vrx_pin = A0;
int vry_pin = A1;

int ir_led_pin = 2;
int joystick_btn_pin = 3;

OneButton JoystickButton(joystick_btn_pin, true, true);

unsigned long lastCommandTime = 0;
unsigned long lastCommand = 0;
unsigned long delayTime = 1000; // ms

void sendIR(unsigned long hex) {
    unsigned long currentTime = millis();
    if (hex != lastCommand || (currentTime - lastCommandTime) >= delayTime) {
        unsigned long newhex = hex;
        if ((newhex & 0xFFF00000) == 0) {
            newhex = (newhex << 12) | 0xFFF;
        }

        IrSender.sendNECMSB(newhex, 32);
        delay(10);

        lastCommand = hex;
        lastCommandTime = currentTime;
    }
}

void joystick() {
    int xValue = analogRead(vrx_pin); 
    int yValue = analogRead(vry_pin);

    if (xValue < 500 || xValue > 550) {
        unsigned long command = (HEX_DRIVE << 12) | (xValue & 0xFFF);
        sendIR(command);
    }

    if (yValue < 500 || yValue > 550) {
        unsigned long command = (HEX_TURN << 12) | (yValue & 0xFFF);
        sendIR(command);
    }

    if (xValue >= 500 && xValue <= 550) {
        sendIR(HEX_MIDDLEX);
    }

    if (yValue >= 500 && yValue <= 550) {
        sendIR(HEX_MIDDLEY);
    }
    
    delay(10);
}

static void clickJoystickButton() {
    //sendIR(something);
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