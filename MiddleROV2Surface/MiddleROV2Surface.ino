#include "GamepadUtils.h"
#include "MovingAverage.h"

#define SERIAL_DEBUG    Serial          // USB Serial port (to PC)
#define SERIAL_CONTROL  Serial1         // Arduino Leonardo
// #define SERIAL_CONTROL  Serial3      // Arudino MEGA

//#define DEBUG_OUTPUT

#define RS485_CONTROL_PIN 2
#define START_BYTE 0xFE
#define END_BYTE 0xEF

const int SAMPLES = 10;
MovingAverage<int, SAMPLES> left;
MovingAverage<int, SAMPLES> right;
MovingAverage<int, SAMPLES> vertical;

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
    int8_t power = constrain(y - x, -MAX_POWER, MAX_POWER);
    left.add(power / getSpeedDivider());
    return left.get();
}

int8_t getRightPower(int y, int x) {
    int8_t power = constrain(y + x, -MAX_POWER, MAX_POWER);
    right.add(power / getSpeedDivider());
    return right.get();
}

int8_t getVerticalPower(int z) {
    int8_t power = -constrain(z, -MAX_POWER, MAX_POWER);
    vertical.add(power / getSpeedDivider());
    return vertical.get();
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
    return checkBtn(buttonR1, buttonR2) * -speed;
}

int8_t getManipulator() {
    int speed = 1;
    return checkBtn(buttonL1, buttonL2) * speed;
}

int8_t getSpeedDivider() {
    int multiplier = checkBtn(buttonSquare, buttonCircle);
    if (multiplier == 0) {
        return 2;
    } else if (multiplier == 1) {
        return 4;
    } else if (multiplier == -1) {
        return 1;
    }
}

void setup() {
    SERIAL_CONTROL.begin(115200);  // control serial port (to ROV)
    SERIAL_DEBUG.begin(115200);   // debug serial port (to PC)

    pinMode(RS485_CONTROL_PIN, OUTPUT);
    digitalWrite(RS485_CONTROL_PIN, HIGH);

    SERIAL_DEBUG.println(F("Starting MiddleROV…"));
}

int8_t getMotorButton() {
	int speed = 1;
	return checkBtn(buttonTriangle, buttonCross) * speed;
}

void loop() {
    readGamepad();

    int y, x, z = 0;
    y = filterAxis(stickLY);
    x = filterAxis(stickLX);
    z = filterAxis(stickRY);

#ifdef DEBUG_OUTPUT
    printButtons();

    SERIAL_DEBUG.print("\t");
    SERIAL_DEBUG.print(getStickState(stickLY)); SERIAL_DEBUG.print("\t");
    SERIAL_DEBUG.print(getStickState(stickLX)); SERIAL_DEBUG.print("\t");
    SERIAL_DEBUG.print(getStickState(stickRY)); SERIAL_DEBUG.print("\t");
    SERIAL_DEBUG.println("");

    SERIAL_DEBUG.print("\t");
    SERIAL_DEBUG.print(y); SERIAL_DEBUG.print("\t");
    SERIAL_DEBUG.print(x); SERIAL_DEBUG.print("\t");
    SERIAL_DEBUG.print(z); SERIAL_DEBUG.print("\t");
    SERIAL_DEBUG.println("");
#endif

    uint8_t buffer[8];
    buffer[0] = START_BYTE;
    buffer[1] = getLeftPower(y, x);
    buffer[2] = getRightPower(y, x);
    buffer[3] = getVerticalPower(z);
    buffer[4] = getCamera();
    buffer[5] = getManipulator();
    buffer[6] = getMotorButton();
    buffer[7] = END_BYTE;

    SERIAL_CONTROL.write(buffer, 8);

    SERIAL_DEBUG.print("\t");
    SERIAL_DEBUG.print((int8_t)buffer[1]); SERIAL_DEBUG.print("\t");
    SERIAL_DEBUG.print((int8_t)buffer[2]); SERIAL_DEBUG.print("\t");
    SERIAL_DEBUG.print((int8_t)buffer[3]); SERIAL_DEBUG.print("\t");
    SERIAL_DEBUG.print((int8_t)buffer[4]); SERIAL_DEBUG.print("\t");
    SERIAL_DEBUG.print((int8_t)buffer[5]); SERIAL_DEBUG.print("\t");
    SERIAL_DEBUG.print((int8_t)buffer[6]); SERIAL_DEBUG.print("\t");
    SERIAL_DEBUG.print(" / "); SERIAL_DEBUG.println(getSpeedDivider());

    delay(50);
}
