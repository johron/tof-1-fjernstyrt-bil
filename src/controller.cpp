#include <Arduino.h>
#include <IRremote.hpp>
#include <OneButton.h>

#include "../hdr/shared.hpp"

const int vry_pin = A0;
const int vrx_pin = A1;
const int ir_led_pin = 2;
const int joystick_btn_pin = 3;

unsigned long oldhex = 0xF00FF00F;

// Function to send IR command
void sendIR(unsigned long hex) {
    unsigned long newhex = hex;

    // Check if the hex number is eight digits long
    if ((newhex & 0xFFF00000) == 0) {
        newhex = (newhex << 12) | 0xFFF; /* Add three hexadecimal digits at the end FFF (dec:4095)
                                            so the number is eight digits long */ 
    }

    if (newhex == oldhex) return;
    oldhex = newhex;
    Serial.print(newhex);
    Serial.print(", ");
    Serial.println(oldhex);

    IrSender.sendNECMSB(newhex, 32); /* 32 because it is a eight digit hexadecimal
                                        number which is 32 bits 8*16=32 */
    delay(10);
}

void joystick() {
    int xValue = analogRead(vrx_pin);
    int yValue = analogRead(vry_pin);

    if (xValue < 450 || xValue > 550) {
        sendIR((HEX_DRIVE << 12) | (xValue & 0xFFF));
    }

    if (yValue < 450 || yValue > 550) {
        sendIR((HEX_TURN << 12) | (yValue & 0xFFF));
    }

    if (yValue >= 450 && yValue <= 550) {
        sendIR(HEX_STOP_T);
    }

    if (xValue >= 450 && xValue <= 550) {
        sendIR(HEX_STOP_D);
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