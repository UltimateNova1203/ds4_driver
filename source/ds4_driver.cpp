#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <signal.h>
#include <time.h>
#include <windows.h>
#include "hidapi.h"

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
BOOL ds4DpadUp = false;
BOOL ds4DpadRight = false;
BOOL ds4DpadDown = false;
BOOL ds4DpadLeft = false;

// Analog Sticks
BYTE ds4LeftStickX = 0;
BYTE ds4LeftStickY = 0;
BYTE ds4RightStickX = 0;
BYTE ds4RightStickY = 0;

// Pressure Sensitive inputs
BYTE ds4L2Pressure = 0;
BYTE ds4R2Pressure = 0;

// Gyro
SHORT ds4GyroX = 0;
SHORT ds4GyroY = 0;
SHORT ds4GyroZ = 0;

// Accel
SHORT ds4AccelX = 0;
SHORT ds4AccelY = 0;
SHORT ds4AccelZ = 0;

// Batter
UCHAR ds4Battery = 0;

// Rumble
BYTE ds4LeftRumble = 0;
BYTE ds4RightRumble = 0;

// Lightbar
BYTE ds4LightRed = 0;
BYTE ds4LightGreen = 0;
BYTE ds4LightBlue = 255;
BYTE ds4LightOnTime = 255;
BYTE ds4LightOffTime = 0;

// DS4 HID Setup
hid_device_info* hidDeviceInfo;
hid_device* hidHandle;

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

    SetConsoleCursorPosition(hStdOut, homeCoords);

    Sleep(25);
}

hid_device_info* currentDevice;
void enumerate_ds4() {
    hid_device_info* currentDevice;
    currentDevice = hid_enumerate(0x054C, 0);
    hidHandle = hid_open(currentDevice->vendor_id, currentDevice->product_id, currentDevice->serial_number);
    if (currentDevice->product_id == 0x054C) {
        printf("Using USB (DS4 v1)    \n");
    }
    if (currentDevice->product_id == 0x09CC) {
        printf("Using USB (DS4 v2)    \n");
    }
    if (currentDevice->product_id == 0x0BA0) {
        printf("Using Wireless Adapter\n");
    }
}

static volatile int keepRunning = 1;

void intHandler(int dummy) {
    keepRunning = 0;
}

int main() {
    hid_init();

    signal(SIGINT, intHandler);
    
    unsigned char inputBuffer[547];
    unsigned char outputBuffer[79];

    memset(inputBuffer, 0, 547);
    memset(outputBuffer, 0, 79);

    ClearScreen();
    while (keepRunning) {
        UpdateScreen();
        enumerate_ds4();

        hid_read(hidHandle, inputBuffer, 547);

        unsigned char reportID = inputBuffer[0];
        ds4LeftStickX = inputBuffer[1];
        ds4LeftStickY = inputBuffer[2];
        ds4RightStickX = inputBuffer[3];
        ds4RightStickY = inputBuffer[4];

        ds4Cross = inputBuffer[5] >> 5 & 0x1;
        ds4Circle = inputBuffer[5] >> 6 & 0x1;
        ds4Triangle = inputBuffer[5] >> 7 & 0x1;
        ds4Square = inputBuffer[5] >> 4 & 0x1;

        ds4L1 = inputBuffer[6] & 0x1;
        ds4L2 = inputBuffer[6] >> 2 & 0x1;
        ds4L3 = inputBuffer[6] >> 6 & 0x1;
        ds4R1 = inputBuffer[6] >> 1 & 0x1;
        ds4R2 = inputBuffer[6] >> 3 & 0x1;
        ds4R3 = inputBuffer[6] >> 7 & 0x1;
        ds4L2Pressure = inputBuffer[8];
        ds4R2Pressure = inputBuffer[9];

        ds4Options = inputBuffer[6] >> 5 & 0x1;
        ds4Touchpad = inputBuffer[7] >> 1 & 0x1;
        ds4Share = inputBuffer[6] >> 4 & 0x1;
        ds4PS = inputBuffer[7] & 0x1;

        ds4GyroX = inputBuffer[13] << 8 | inputBuffer[14];
        ds4GyroY = inputBuffer[15] << 8 | inputBuffer[16];
        ds4GyroZ = inputBuffer[17] << 8 | inputBuffer[18];

        ds4AccelX = inputBuffer[19] << 8 | inputBuffer[20];
        ds4AccelY = inputBuffer[21] << 8 | inputBuffer[22];
        ds4AccelZ = inputBuffer[23] << 8 | inputBuffer[24];

        ds4Battery = inputBuffer[29] & 0xF;

        if (ds4Touchpad) {
            ds4LeftRumble = ds4L2Pressure;
            ds4RightRumble = ds4R2Pressure;

            if (ds4L1 == true) {
                ds4LightRed = 255;
                ds4LightGreen = 0;
                ds4LightBlue = 0;
            }
            else if (ds4R1 == true) {
                ds4LightRed = 0;
                ds4LightGreen = 255;
                ds4LightBlue = 0;
            }
            else {
                ds4LightRed = 0;
                ds4LightGreen = 0;
                ds4LightBlue = 255;
            }

            if (ds4DpadUp == true) {
                ds4LightOnTime++;
            }
            else if (ds4DpadDown == true) {
                ds4LightOnTime--;
            }

            if (ds4DpadLeft == true) {
                ds4LightOffTime--;
            }
            else if (ds4DpadRight == true) {
                ds4LightOffTime++;
            }
        }
        else { ds4LeftRumble = 0; ds4RightRumble = 0; }


        if ((inputBuffer[5] & 0xF) == 0) { ds4DpadUp = true; ds4DpadLeft = false; ds4DpadDown = false; ds4DpadRight = false; }
        if ((inputBuffer[5] & 0xF) == 1) { ds4DpadUp = true; ds4DpadLeft = false; ds4DpadDown = false; ds4DpadRight = true; }
        if ((inputBuffer[5] & 0xF) == 2) { ds4DpadUp = false; ds4DpadLeft = false; ds4DpadDown = false; ds4DpadRight = true; }
        if ((inputBuffer[5] & 0xF) == 3) { ds4DpadUp = false; ds4DpadLeft = false; ds4DpadDown = true; ds4DpadRight = true; }
        if ((inputBuffer[5] & 0xF) == 4) { ds4DpadUp = false; ds4DpadLeft = false; ds4DpadDown = true; ds4DpadRight = false; }
        if ((inputBuffer[5] & 0xF) == 5) { ds4DpadUp = false; ds4DpadLeft = true; ds4DpadDown = true; ds4DpadRight = false; }
        if ((inputBuffer[5] & 0xF) == 6) { ds4DpadUp = false; ds4DpadLeft = true; ds4DpadDown = false; ds4DpadRight = false; }
        if ((inputBuffer[5] & 0xF) == 7) { ds4DpadUp = true; ds4DpadLeft = true; ds4DpadDown = false; ds4DpadRight = false; }
        if ((inputBuffer[5] & 0xF) == 8) { ds4DpadUp = false; ds4DpadLeft = false; ds4DpadDown = false; ds4DpadRight = false; }

        outputBuffer[0] = 0x05;
        outputBuffer[1] = 0xF7;
        outputBuffer[2] = 0x04;

        outputBuffer[4] = ds4RightRumble; // Right Motor
        outputBuffer[5] = ds4LeftRumble;  // Left Motor

        outputBuffer[6] = ds4LightRed;  // Red LED
        outputBuffer[7] = ds4LightGreen;  // Green LED
        outputBuffer[8] = ds4LightBlue;  // Blue LED

        outputBuffer[9] = ds4LightOnTime;  // LED Time On
        outputBuffer[10] = ds4LightOffTime; // LED Time Off

        printf("Report ID: %d\n\n", reportID);
        printf("     Left Stick     Right Stick\n");
        printf("       X: %03d          X: %03d\n", ds4LeftStickX, ds4RightStickX);
        printf("       Y: %03d          Y: %03d\n\n", ds4LeftStickY, ds4RightStickY);

        printf("    Face Buttons    Dpad Buttons\n");
        printf("     Cross: %01d         Up: %01d\n", ds4Cross, ds4DpadUp);
        printf("    Circle: %01d       Left: %01d\n", ds4Circle, ds4DpadLeft);
        printf("  Triangle: %01d       Down: %01d\n", ds4Triangle, ds4DpadDown);
        printf("    Square: %01d      Right: %01d\n\n", ds4Square, ds4DpadRight);

        printf("    Left Shoulder    Right Shoulder\n");
        printf("        L1: %01d         R1: %01d\n", ds4L1, ds4R1);
        printf("        L2: %01d         R2: %01d\n", ds4L2, ds4R2);
        printf("        L3: %01d         R3: %01d\n", ds4L3, ds4R3);
        printf("        L3: %03d       R3: %03d\n\n", ds4L2Pressure, ds4R2Pressure);

        printf("         Special Buttons\n");
        printf("     Options: %01d   Touchpad: %01d\n", ds4Options, ds4Touchpad);
        printf("       Share: %01d         PS: %01d\n\n", ds4Share, ds4PS);

        printf("         Gyro         Accel\n");
        printf("      X: %06d     X: %06d\n", ds4GyroX, ds4AccelX);
        printf("      Y: %06d     Y: %06d\n", ds4GyroY, ds4AccelY);
        printf("      Z: %06d     Z: %06d\n\n", ds4GyroZ, ds4AccelZ);

        printf("     Battery: %02d\n", ds4Battery);
        printf("  Left Motor: %03d\n", ds4LeftRumble);
        printf(" Right Motor: %03d\n\n", ds4RightRumble);
        printf("     LED Red: %03d\n", ds4LightRed);
        printf("   LED Green: %03d\n", ds4LightGreen);
        printf("    LED Blue: %03d\n", ds4LightBlue);
        printf(" LED Time On: %03d\n", ds4LightOnTime);
        printf("LED Time Off: %03d\n", ds4LightOffTime);

        hid_set_nonblocking(hidHandle, 0);
        hid_write(hidHandle, outputBuffer, 79);
    }
    
    outputBuffer[0] = 0x05;
    outputBuffer[1] = 0xF7;
    outputBuffer[2] = 0x04;

    outputBuffer[4] = 0x00;  // Left Motor
    outputBuffer[5] = 0x00;  // Right Motor

    outputBuffer[6] = 0x00;  // Red LED
    outputBuffer[7] = 0x00;  // Green LED
    outputBuffer[8] = 0x00;  // Blue LED

    outputBuffer[9] = 0x00;  // LED Time On
    outputBuffer[10] = 0xFF; // LED Time Off

    hid_set_nonblocking(hidHandle, 0);
    hid_write(hidHandle, outputBuffer, 78);

    hid_close(hidHandle);
    hid_exit();

    printf("\nClosed handle");
    return 0;
}