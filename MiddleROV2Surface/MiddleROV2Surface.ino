/*
 Name:		MiddleROV2Surface.ino
 Created:	06.12.2018 17:37:33
 Author:	Zinkov
*/

#include "GamepadUtils.h"

#define DEBUG

#define RS485_CONTROL_PIN 2
#define START_BYTE 0xFE
#define END_BYTE 0xEF

constexpr int MAX_POWER = 100;

int treshold(int v, int b) {
    if (abs(v) < b) return 0;
    return v;
}

int filterAxis(byte axis) {
    int an = getStickState(axis);
    an -= 127;
    an = treshold(an, 10) * -1;
    an = constrain(an, -127, 127);
    int val = map(an, -127, 127, -MAX_POWER, MAX_POWER);
    return val;
}

int8_t getLeftPower(int y, int x) {
    return constrain(y - x, -MAX_POWER, MAX_POWER);
}

int8_t getRightPower(int y, int x) {
    return constrain(y + x, -MAX_POWER, MAX_POWER);
}

int8_t getVerticalPower(int z) {
    return constrain(z, -MAX_POWER, MAX_POWER)*-1;
}

int8_t checkBtn(uint16_t btn1, uint16_t btn2) {
    if (getButtonState(btn1)) {
        return 1;
    }
    if (getButtonState(btn2)) {
        return -1;
    }
    return 0;
}

int8_t getCamera() {
    int speed = 1;
    return checkBtn(buttonR1, buttonR2) * speed;
}

int8_t getManipulator() {
    int speed = 1;
    return checkBtn(buttonL1, buttonL2) * speed;
}

void setup() {
    Serial.begin(115200);   // control serial port (to ROV)
    Serial1.begin(115200);  // debug serial port (to PC)

    pinMode(RS485_CONTROL_PIN, OUTPUT);
    digitalWrite(RS485_CONTROL_PIN, HIGH);

    Serial.println(F("Starting MiddleROV…"));
}

void loop() {
    readGamepad();

    int y, x, z = 0;
    y = filterAxis(stickLY);
    x = filterAxis(stickLX);
    z = filterAxis(stickRY);

#ifdef DEBUG
    printButtons();

    Serial.print("\t");
    Serial.print(getStickState(stickLY)); Serial.print("\t");
    Serial.print(getStickState(stickLX)); Serial.print("\t");
    Serial.print(getStickState(stickRY)); Serial.print("\t");
    Serial.println("");

    Serial.print("\t");
    Serial.print(y); Serial.print("\t");
    Serial.print(x); Serial.print("\t");
    Serial.print(z); Serial.print("\t");
    Serial.println("");
#endif

    uint8_t buffer[7];
    buffer[0] = START_BYTE;
    buffer[1] = getLeftPower(y, x);
    buffer[2] = getRightPower(y, x);
    buffer[3] = getVerticalPower(z);
    buffer[4] = getCamera();
    buffer[5] = getManipulator();
    buffer[6] = END_BYTE;
    Serial1.write(buffer, 7);
    Serial.print("\t");
    Serial.print((int8_t)buffer[1]); Serial.print("\t");
    Serial.print((int8_t)buffer[2]); Serial.print("\t");
    Serial.print((int8_t)buffer[3]); Serial.print("\t");
    Serial.print((int8_t)buffer[4]); Serial.print("\t");
    Serial.println((int8_t)buffer[5]);
    delay(50);
}
