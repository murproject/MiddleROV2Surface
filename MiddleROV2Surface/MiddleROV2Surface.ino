/*
 Name:		MiddleROV2Surface.ino
 Created:	06.12.2018 17:37:33
 Author:	Zinkov
*/

// the setup function runs once when you press reset or power the board
#include "PS2X_lib.h"

#define PS2_DAT        13  //14
#define PS2_CMD        11  //15
#define PS2_SEL        10  //16
#define PS2_CLK        12  //17

//#define pressures   true
#define pressures   false
//#define rumble      true
#define rumble      false

#define RS485_CONTROL_PIN 2

#define START_BYTE 0xFE
#define END_BYTE 0xEF

#define DEBUG

PS2X ps2x;

int error = 0;
byte type = 0;
byte vibrate = 0;

int treshold(int v, int b) {
    if (abs(v) < b) return 0;
    return v;
}

constexpr int MAX_POWER = 100;

int filterAxis(byte axis) {
    int an = ps2x.Analog(axis);
    an -= 127;
    an = treshold(an, 2) * -1;
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
    if (ps2x.Button(btn1)) {
        return 1;
    }
    if (ps2x.Button(btn2)) {
        return -1;
    }
    return 0;
}

int8_t getCamera() {
    int speed = 1;
    return checkBtn(PSB_R1, PSB_R2) * speed;
}

int8_t getManipulator() {
    int speed = 1;
    return checkBtn(PSB_L1, PSB_L2) * speed;
}

void setup() {
    Serial.begin(115200);
    Serial1.begin(115200);

    pinMode(RS485_CONTROL_PIN, OUTPUT);
    digitalWrite(RS485_CONTROL_PIN, HIGH);

    error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
    type = ps2x.readType();

}

void loop() {
    ps2x.read_gamepad(false, vibrate);
    int y, x, z = 0;
    y = filterAxis(PSS_LY);
    x = filterAxis(PSS_LX);
    z = filterAxis(PSS_RY);

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
