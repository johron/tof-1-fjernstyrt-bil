#include <Arduino.h>
#include <IRremote.hpp>
#include <OneButton.h>
#include <shared.h>

int vry_pin = A0;
int vrx_pin = A1;

int ir_led_pin = 2;
int joystick_btn_pin = 3;

OneButton JoystickButton(joystick_btn_pin, true, true);

unsigned long lastCommandTime = 0;
unsigned long lastCommand = 0xF00FF00F; // defualt
unsigned long delayTime = 1000; // ms

void sendIR(unsigned long hex) {
    unsigned long currentTime = millis();
    unsigned long newhex = hex;
    if (newhex != lastCommand || (currentTime - lastCommandTime) >= delayTime) {
    if ((newhex & 0xFFF00000) == 0) {
        newhex = (newhex << 12) | 0xFFF;
    }

        IrSender.sendNECMSB(newhex, 32);
        delay(10);

        lastCommand = newhex;
        lastCommandTime = currentTime;
    }
}

void joystick() {
    int xValue = analogRead(vrx_pin); 
    int yValue = analogRead(vry_pin);

    if (xValue < 450 || xValue > 600) {
        unsigned long command = (HEX_DRIVE << 12) | (xValue & 0xFFF);
        sendIR(command);
    }

    if (yValue < 450 || yValue > 600) {
        unsigned long command = (HEX_TURN << 12) | (yValue & 0xFFF);
        sendIR(command);
    }

    if (xValue >= 450 && xValue <= 600) {
        sendIR(HEX_MIDDLEX);
    }

    if (yValue >= 450 && yValue <= 600) {
        sendIR(HEX_MIDDLEY);
    }
    
    delay(10);
}

static void clickJoystickButton() {
    sendIR(HEX_LIGHTS);
}

void buttons() {
    JoystickButton.attachClick(clickJoystickButton);
    JoystickButton.tick();
}

void test() {
    sendIR((HEX_DRIVE << 12) | (1022 & 0xFFF));
    delay(20);
    sendIR((HEX_TURN << 12) | (1022 & 0xFFF));
    delay(1000);
    sendIR((HEX_MIDDLEX << 12) | (255 & 0xFFF));
    delay(20);
    sendIR((HEX_MIDDLEY << 12) | (255 & 0xFFF));
    delay(1000);
    sendIR((HEX_DRIVE << 12) | (0 & 0xFFF));
    delay(20);
    sendIR((HEX_TURN << 12) | (0 & 0xFFF));
    delay(1000);
    sendIR((HEX_MIDDLEX << 12) | (255 & 0xFFF));
    delay(20);
    sendIR((HEX_MIDDLEY << 12) | (255 & 0xFFF));
    delay(1000);
}

void loop() {
    joystick();
    buttons();
    //test();
}

void setup() {
    Serial.begin(9600);
    IrSender.begin(ir_led_pin);
}