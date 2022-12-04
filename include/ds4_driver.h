#pragma once

extern BOOL ds4IsPresent;

// Digital Inputs
extern BOOL ds4Cross;
extern BOOL ds4Circle;
extern BOOL ds4Triangle;
extern BOOL ds4Square;
extern BOOL ds4L1;
extern BOOL ds4L2;
extern BOOL ds4L3;
extern BOOL ds4R1;
extern BOOL ds4R2;
extern BOOL ds4R3;
extern BOOL ds4Options;
extern BOOL ds4Share;
extern BOOL ds4Touchpad;
extern BOOL ds4PS;

// Dpad
extern BOOL ds4DpadUp;
extern BOOL ds4DpadRight;
extern BOOL ds4DpadDown;
extern BOOL ds4DpadLeft;

// Analog Sticks
extern BYTE ds4LeftStickX;
extern BYTE ds4LeftStickY;
extern BYTE ds4RightStickX;
extern BYTE ds4RightStickY;

// Pressure Sensitive inputs
extern BYTE ds4L2Pressure;
extern BYTE ds4R2Pressure;

// Rumble
extern BYTE ds4LeftMotor;
extern BYTE ds4RightMotor;

// Acceleration
extern SHORT ds4AccelX;
extern SHORT ds4AccelY;
extern SHORT ds4AccelZ;

// Gyro
extern SHORT ds4GyroX;
extern SHORT ds4GyroY;
extern SHORT ds4GyroZ;

typedef struct _DS4_LIGHTBAR {
    BYTE Red = 0;
    BYTE Green = 0;
    BYTE Blue = 0;
} DS4_LIGHTBAR;

typedef struct _DS4_VIBRATION {
    BYTE Strong = 0;
    BYTE Weak = 0;
} DS4_VIBRATION;