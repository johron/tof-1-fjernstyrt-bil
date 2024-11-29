#include <Arduino.h>
#include <IRremote.hpp>
#include <OneButton.h>
#include <shared.hpp>

int vry_pin = A0; /* In this project i use the X axis as the throttle because that's
                     how i prefer to hold it, so X is vertical and Y is horizontal */
int vrx_pin = A1;
int ir_led_pin = 2;
int joystick_btn_pin = 3;

OneButton JoystickButton(joystick_btn_pin, true, true);

unsigned long lastCommandTime = 0;
unsigned long lastCommand = 0xF00FF00F;
unsigned long delayTime = 1000;

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

    // Check if the joystick is moved in the x direction
    if (xValue < 450 || xValue > 600) {
        unsigned long command = (HEX_DRIVE << 12) | (xValue & 0xFFF); /* Add the value in the direction as 3 hexadecimal digits
                                                                         at the end of the operation hex code */
        sendIR(command);
    }

    // Check if the joystick is moved in the y direction
    if (yValue < 450 || yValue > 600) {
        unsigned long command = (HEX_TURN << 12) | (yValue & 0xFFF);
        sendIR(command);
    }

    // Check if the joystick is centered in the x direction
    if ((xValue >= 450 && xValue <= 600) || (yValue >= 450 && yValue <= 600)) {
        sendIR(HEX_MIDDLE);
    } 

    delay(10);
}

static void clickJoystickButton() {
    sendIR(HEX_HEADLIGHTS);
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
    sendIR((HEX_MIDDLE << 12) | (255 & 0xFFF));
    delay(1000);
    sendIR((HEX_DRIVE << 12) | (0 & 0xFFF));
    delay(20);
    sendIR((HEX_TURN << 12) | (0 & 0xFFF));
    delay(1000);
    sendIR((HEX_MIDDLE << 12) | (255 & 0xFFF));
    delay(1000);
}

void loop() {
    test();
    //joystick();
    buttons();
}

void setup() {
    Serial.begin(9600);
    IrSender.begin(ir_led_pin);
}