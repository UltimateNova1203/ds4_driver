#pragma once
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "hidapi.h"
#include "JoyShockLibrary.h"
#include "ds4_driver.h"

BOOL ds4IsPresent = false;
BOOL keepRunning = true;

int connectedDevices = 0;
int deviceHandles = 0;
int controllerType = 0;

#pragma region Inputs
// Digital Inputs
BOOL ds4Cross = false;
BOOL ds4Circle = false;
BOOL ds4Triangle = false;
BOOL ds4Square = false;
BOOL ds4L1 = false;
BOOL ds4L2 = false;
BOOL ds4L3 = false;
BOOL ds4R1 = false;
BOOL ds4R2 = false;
BOOL ds4R3 = false;
BOOL ds4Options = false;
BOOL ds4Share = false;
BOOL ds4Touchpad = false;
BOOL ds4PS = false;

// Dpad
BOOL ds4DpadUp = false;
BOOL ds4DpadLeft = false;
BOOL ds4DpadDown = false;
BOOL ds4DpadRight = false;

// Analog Sticks
BYTE ds4LeftStickX = 128;
BYTE ds4LeftStickY = 128;
BYTE ds4RightStickX = 128;
BYTE ds4RightStickY = 128;

// Pressure Sensitive inputs
BYTE ds4L2Pressure = 0;
BYTE ds4R2Pressure = 0;

// Rumble
BYTE ds4LeftMotor = 0;
BYTE ds4RightMotor = 0;

// Acceleration
SHORT ds4AccelX = 0;
SHORT ds4AccelY = 0;
SHORT ds4AccelZ = 0;
SHORT ds4PrevAccelX = 0;
SHORT ds4PrevAccelY = 0;
SHORT ds4PrevAccelZ = 0;

// Gyro
SHORT ds4GyroX = 0;
SHORT ds4GyroY = 0;
SHORT ds4GyroZ = 0;
SHORT ds4PrevGyroX = 0;
SHORT ds4PrevGyroY = 0;
SHORT ds4PrevGyroZ = 0;
#pragma endregion

void intHandler(int dummy) {
    keepRunning = false;
}

void ClearScreen()
{
    HANDLE                     hStdOut;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD                      count;
    DWORD                      cellCount;
    COORD                      homeCoords = { 0, 0 };

    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut == INVALID_HANDLE_VALUE) return;

    /* Get the number of cells in the current buffer */
    if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;
    cellCount = csbi.dwSize.X * csbi.dwSize.Y;

    /* Fill the entire buffer with spaces */
    if (!FillConsoleOutputCharacter(
        hStdOut,
        (TCHAR)' ',
        cellCount,
        homeCoords,
        &count
    )) return;

    /* Fill the entire buffer with the current colors and attributes */
    if (!FillConsoleOutputAttribute(
        hStdOut,
        csbi.wAttributes,
        cellCount,
        homeCoords,
        &count
    )) return;

    /* Move the cursor home */
    SetConsoleCursorPosition(hStdOut, homeCoords);
}

void UpdateScreen()
{
    HANDLE                     hStdOut;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD                      count;
    DWORD                      cellCount;
    COORD                      homeCoords = { 0, 0 };

    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut == INVALID_HANDLE_VALUE) return;

    /* Get the number of cells in the current buffer */
    if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;
    cellCount = csbi.dwSize.X * csbi.dwSize.Y;

    /* Fill the entire buffer with the current colors and attributes */
    if (!FillConsoleOutputAttribute(
        hStdOut,
        csbi.wAttributes,
        cellCount,
        homeCoords,
        &count
    )) return;

    /* Move the cursor home */
    SetConsoleCursorPosition(hStdOut, homeCoords);
}

BYTE float2byte(FLOAT floatValue) {
    BYTE newOutput = (floatValue) * 255;
    return newOutput;
}

BYTE float2byteCentered(FLOAT floatValue) {
    BYTE newOutput = (floatValue * 0.5 + 0.5) * 255;
    return newOutput;
}

BYTE invertStick(BOOL IsUpDown, BYTE stickValue) {
    BYTE newStickValue = stickValue;

    if (IsUpDown == true) {
        newStickValue = 1 + stickValue;
    }
    else {
        newStickValue = -1 - stickValue;
    }

    return newStickValue;
}

BYTE stickDeadzone(BYTE stickValue, BYTE deadzonePercent) {
    BYTE lowerBound = 128 - ((0.01 * deadzonePercent) * 255);
    BYTE upperBound = 128 + ((0.01 * deadzonePercent) * 255);

    if (stickValue >= lowerBound && stickValue <= upperBound) {
        return 128;
    }
    else {
        return stickValue;
    }
}

void jslUpdate() {
    connectedDevices = JslConnectDevices();
    deviceHandles = JslGetConnectedDeviceHandles (&connectedDevices, connectedDevices);
}

INT ds4LightbarColor(_DS4_LIGHTBAR Lightbar) {
    INT mixedColor = ((Lightbar.Red << 16) | (Lightbar.Green << 8) | Lightbar.Blue);
    return mixedColor;
}

void DS4GetState() {
    JOY_SHOCK_STATE DS4_STATE = JslGetSimpleState(controllerType);

    if (DS4_STATE.buttons & JSMASK_S) { ds4Cross = true; } else { ds4Cross = false; }
    if (DS4_STATE.buttons & JSMASK_E) { ds4Circle = true; } else { ds4Circle = false; }
    if (DS4_STATE.buttons & JSMASK_N) { ds4Triangle = true; } else { ds4Triangle = false; }
    if (DS4_STATE.buttons & JSMASK_W) { ds4Square = true; } else { ds4Square = false; }

    if (DS4_STATE.buttons & JSMASK_UP) { ds4DpadUp = true; } else { ds4DpadUp = false; }
    if (DS4_STATE.buttons & JSMASK_LEFT) { ds4DpadLeft = true; } else { ds4DpadLeft = false; }
    if (DS4_STATE.buttons & JSMASK_DOWN) { ds4DpadDown = true; } else { ds4DpadDown = false; }
    if (DS4_STATE.buttons & JSMASK_RIGHT) { ds4DpadRight = true; } else { ds4DpadRight = false; }

    if (DS4_STATE.buttons & JSMASK_L) { ds4L1 = true; } else { ds4L1 = false; }
    if (DS4_STATE.buttons & JSMASK_ZL) { ds4L2 = true; } else { ds4L2 = false; }
    if (DS4_STATE.buttons & JSMASK_LCLICK) { ds4L3 = true; } else { ds4L3 = false; }
    if (DS4_STATE.buttons & JSMASK_R) { ds4R1 = true; } else { ds4R1 = false; }
    if (DS4_STATE.buttons & JSMASK_ZR) { ds4R2 = true; } else { ds4R2 = false; }
    if (DS4_STATE.buttons & JSMASK_RCLICK) { ds4R3 = true; } else { ds4R3 = false; }

    if (DS4_STATE.buttons & JSMASK_PLUS) { ds4Options = true; } else { ds4Options = false; }
    if (DS4_STATE.buttons & JSMASK_MINUS) { ds4Share = true; } else { ds4Share = false; }
    if (DS4_STATE.buttons & JSMASK_CAPTURE) { ds4Touchpad = true; } else { ds4Touchpad = false; }
    if (DS4_STATE.buttons & JSMASK_HOME) { ds4PS = true; } else { ds4PS = false; }

    ds4LeftStickX = float2byteCentered(DS4_STATE.stickLX);
    ds4LeftStickY = float2byteCentered(DS4_STATE.stickLY);
    ds4RightStickX = float2byteCentered(DS4_STATE.stickRX);
    ds4RightStickY = float2byteCentered(DS4_STATE.stickRY);

    ds4LeftStickY = invertStick(false, ds4LeftStickY);
    ds4RightStickY = invertStick(false, ds4RightStickY);

    ds4LeftStickX = stickDeadzone(ds4LeftStickX, 5);
    ds4LeftStickY = stickDeadzone(ds4LeftStickY, 5);
    ds4RightStickX = stickDeadzone(ds4RightStickX, 5);
    ds4RightStickY = stickDeadzone(ds4RightStickY, 5);

    ds4L2Pressure = float2byte(DS4_STATE.lTrigger);
    ds4R2Pressure = float2byte(DS4_STATE.rTrigger);
}

void DS4SetState(_DS4_VIBRATION Vibration, _DS4_LIGHTBAR Lightbar) {
    JslSetRumble(controllerType, Vibration.Weak, Vibration.Strong);
    //JslSetLightColour(controllerType, ds4LightbarColor(Lightbar));
}

int main() {
    ClearScreen();

    JslDisconnectAndDisposeAll;

    DS4_LIGHTBAR ds4Lightbar;
    ds4Lightbar.Red = 0;
    ds4Lightbar.Green = 0;
    ds4Lightbar.Blue = 255;

    DS4_VIBRATION ds4Vibration;
    ds4Vibration.Strong = 0;
    ds4Vibration.Weak = 0;

    jslUpdate();
    DS4GetState();
    DS4SetState(ds4Vibration, ds4Lightbar);
    JslSetLightColour(controllerType, ds4LightbarColor(ds4Lightbar));

    while (keepRunning) {
        UpdateScreen();
        DS4GetState();

        printf("Sticks:\n");
        printf("|  LX: %3d  |  LY: %3d  |  L3: %01d\n", ds4LeftStickX, ds4LeftStickY, ds4L3);
        printf("|  RX: %3d  |  RY: %3d  |  R3: %01d\n\n", ds4RightStickX, ds4RightStickY, ds4R3);

        printf("Face:\n");
        printf("|  Cross: %01d  |  Circle: %01d  |  Triangle: %01d  |  Square: %01d\n", ds4Cross, ds4Circle, ds4Triangle, ds4Square);
        printf("|     Up: %01d  |    Left: %01d  |      Down: %01d  |   Right: %01d\n\n", ds4DpadUp, ds4DpadLeft, ds4DpadDown, ds4DpadRight);

        printf("Shoulders:\n");
        printf("|  L1: %01d |  L2: %01d |  Pressure: %3d\n", ds4L1, ds4L2, ds4L2Pressure);
        printf("|  R1: %01d |  R2: %01d |  Pressure: %3d\n\n", ds4R1, ds4R2, ds4R2Pressure);

        printf("Special:\n");
        printf("|  Options: %01d  |  Share: %01d  |  Touchpad: %01d  |  PS: %01d\n\n", ds4Options, ds4Share, ds4Touchpad, ds4PS);

        printf("Rumble:\n");
        printf("|  Left Motor: %3d  |  Right Motor: %3d\n\n", ds4Vibration.Strong, ds4Vibration.Weak);

        printf("Lightbar:\n");
        printf("|      Red: %3d  |     Green: %3d  |  Blue: %3d\n", ds4Lightbar.Red, ds4Lightbar.Green, ds4Lightbar.Blue);

        printf("Gyro:\n");
        printf("|  X: %6d  |  Y: %6d  |  Z: %6d\n\n", ds4GyroX, ds4GyroY, ds4GyroZ);

        printf("Acceleration:\n");
        printf("|  X: %6d  |  Y: %6d  |  Z: %6d\n", ds4AccelX, ds4AccelY, ds4AccelZ);
        //printf("     Battery: %02d\n", ds4Battery);

        if (ds4Touchpad && (ds4L2Pressure || ds4R2Pressure)) {
            ds4Vibration.Strong = ds4L2Pressure;
            ds4Vibration.Weak = ds4R2Pressure;
        }

        else {
            ds4Vibration.Strong = 0;
            ds4Vibration.Weak = 0;
        }

        if (ds4Options && ds4Share == true) {
            keepRunning = false;
        }

        if (ds4Touchpad && ds4L1 == true && ds4R1 == false) {
            ds4Lightbar.Red = ds4L2Pressure;
        }

        if (ds4Touchpad && ds4R1 == true && ds4L1 == false) {
            ds4Lightbar.Green = ds4R2Pressure;
        }

        if (ds4Touchpad && ds4L1 && ds4R1 == true) {
            ds4Lightbar.Blue = ds4L2Pressure;
        }

        DS4SetState(ds4Vibration, ds4Lightbar);
        Sleep(33);
    }
    ClearScreen();

    ds4Lightbar.Red = 16;
    ds4Lightbar.Green = 24;
    ds4Lightbar.Blue = 32;
    ds4Vibration.Strong = 0;
    ds4Vibration.Weak = 0;
    DS4SetState(ds4Vibration, ds4Lightbar);
    JslSetLightColour(controllerType, ds4LightbarColor(ds4Lightbar));

    JslDisconnectAndDisposeAll;
    printf("Closed handler\n");

    return 0;
}
