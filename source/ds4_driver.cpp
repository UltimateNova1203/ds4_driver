#pragma once
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "hidapi.h"
#include "ds4_driver.h"

const wchar_t *hidError;

hid_device_info* hidDevice = hid_enumerate(0x054C, 0);
hid_device* hidHandle = hid_open(hidDevice->vendor_id, hidDevice->product_id, hidDevice->serial_number);

UCHAR inputBuffer[100];
UCHAR outputBuffer[80];
SHORT DS4_PID = 0;
BOOL ds4IsPresent = false;
BOOL ds4IsWireless = false;
BOOL keepRunning = true;

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

// Lightbar
BYTE ds4LightRed = 0;
BYTE ds4LightGreen = 0;
BYTE ds4LightBlue = 255;
BYTE ds4LightOnTime = 255;
BYTE ds4LightOffTime = 0;

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

void DS4Enumeration() {
    ClearScreen();

    printf("Enumerating\n");
    printf("HID Exit\n");
    hid_exit();
    Sleep(250);

    printf("HID Init\n");
    hid_init();
    Sleep(250);

    printf("HID Enumerate: DS4 v1\n");
    hidDevice = hid_enumerate(0x054C, 0x054C);
    DS4_PID = 0x054C;
    Sleep(250);

    if (hidDevice == NULL) {
        //hid_free_enumeration(hidDevice);
        printf("Couldn't get DS4 v1\n");

        printf("HID Enumerate: DS4 v2\n");
        hidDevice = hid_enumerate(0x054C, 0x09CC);
        DS4_PID = 0x09CC;
        Sleep(250);

        if (hidDevice == NULL) {
            printf("Couldn't get DS4 v2\n");

            printf("HID Enumerate: Wireless Adapter\n");
            hidDevice = hid_enumerate(0x054C, 0x0BA0);
            DS4_PID = 0x0BA0;
            Sleep(250);

            if (hidDevice == NULL) {
                printf("Couldn't get Wireless Adapter\n");

                printf("HID Enumerate: Bluetooth\n");
                hidDevice = hid_enumerate(0x054C, 0x08C1);
                DS4_PID = 0x08C1;
                Sleep(250);

                if (hidDevice == NULL) {
                    printf("Couldn't get Bluetooth\n");
                    Sleep(250);
                }
            }
        }

        Sleep(1000);
    }
    if (hidDevice != NULL) {
        printf("HID Open\n");
        hidHandle = hid_open(0x054C, DS4_PID, hidDevice->serial_number);
        //hidHandle = hid_open(0x54C, hidDevice->product_id, NULL);
        hidError = hid_error(hidHandle);
        printf("%ls\n", hidError);
        Sleep(250);
        hidError = L"SUCCESS";
        ds4IsPresent = true;
        ClearScreen();
    }
}

void DS4Presence(hid_device* hidHandle) {
    hidHandle = hid_open(0x054C, DS4_PID, NULL);
    hidError = hid_error(hidHandle);
    printf("%ls: ", hidError);

    if (ds4IsWireless == true) {
        printf("Using BlueTooth\n");
    }
    if (ds4IsWireless == false) {
        if (DS4_PID == 0x0BA0) {
            printf("Using Wireless Adapter\n");
        }
        else {
            printf("Using USB\n");
        }
    }

    if (hidError != L"Success") {
        ClearScreen();
        printf("DS4 Is Not Present\n");
        printf("%ls\n", hidError);
        Sleep(1000);
        ds4IsPresent = false;
    }
}

void DS4GetStateUSB(hid_device* hidHandle) {
    memset(inputBuffer, 0, 31);
    hidHandle = hid_open(0x54C, DS4_PID, NULL);
    hid_read(hidHandle, inputBuffer, 100);

    ds4Cross = inputBuffer[5] >> 5 & 0x1;
    ds4Circle = inputBuffer[5] >> 6 & 0x1;
    ds4Triangle = inputBuffer[5] >> 7 & 0x1;
    ds4Square = inputBuffer[5] >> 4 & 0x1;

    BYTE ds4Dpad = inputBuffer[5] & 0xF;

    if (ds4Dpad == 0 || ds4Dpad == 1 || ds4Dpad == 7) { ds4DpadUp = true; }
    else { ds4DpadUp = false; }
    if (ds4Dpad == 5 || ds4Dpad == 6 || ds4Dpad == 7) { ds4DpadLeft = true; }
    else { ds4DpadLeft = false; }
    if (ds4Dpad == 3 || ds4Dpad == 4 || ds4Dpad == 5) { ds4DpadDown = true; }
    else { ds4DpadDown = false; }
    if (ds4Dpad == 1 || ds4Dpad == 2 || ds4Dpad == 3) { ds4DpadRight = true; }
    else { ds4DpadRight = false; }
    if (ds4Dpad == 8) {
        ds4DpadUp = false;
        ds4DpadLeft = false;
        ds4DpadDown = false;
        ds4DpadRight = false;
    }

    ds4LeftStickX = inputBuffer[1];
    ds4LeftStickY = inputBuffer[2];
    ds4RightStickX = inputBuffer[3];
    ds4RightStickY = inputBuffer[4];

    // Analog Stick Deadzone
    if (ds4LeftStickX >= 112 && ds4LeftStickX <= 144) {
    ds4LeftStickX = 128;
    }
    if (ds4LeftStickY >= 112 && ds4LeftStickY <= 144) {
    ds4LeftStickY = 128;
    }
    if (ds4RightStickX >= 112 && ds4RightStickX <= 144) {
    ds4RightStickX = 128;
    }
    if (ds4RightStickY >= 112 && ds4RightStickY <= 144) {
    ds4RightStickY = 128;
    }

    ds4L1 = inputBuffer[6] & 0x1;
    ds4L2 = inputBuffer[6] >> 2 & 0x1;
    ds4L3 = inputBuffer[6] >> 6 & 0x1;
    ds4R1 = inputBuffer[6] >> 1 & 0x1;
    ds4R2 = inputBuffer[6] >> 3 & 0x1;
    ds4R3 = inputBuffer[6] >> 7 & 0x1;
    ds4Options = inputBuffer[6] >> 5 & 0x1;
    ds4Share = inputBuffer[6] >> 4 & 0x1;

    ds4Touchpad = inputBuffer[7] >> 1 & 0x1;
    ds4PS = inputBuffer[7] & 0x1;

    ds4L2Pressure = inputBuffer[8];
    ds4R2Pressure = inputBuffer[9];

    // Trigger Deadzone
    if (ds4L2Pressure >= 0 && ds4L2Pressure <= 32) {
    ds4L2Pressure = 0;
    }
    if (ds4R2Pressure >= 0 && ds4R2Pressure <= 32) {
    ds4R2Pressure = 0;
    }

    // Trigger Anti Deadzone
    if (ds4L2Pressure >= 200 && ds4L2Pressure <= 255) {
    ds4L2Pressure = 200;
    }
    if (ds4R2Pressure >= 200 && ds4R2Pressure <= 255) {
    ds4R2Pressure = 200;
    }

    ds4GyroX = ((inputBuffer[13] << 8 | inputBuffer[14]) / 16);
    ds4GyroY = ((inputBuffer[15] << 8 | inputBuffer[16]) / 16);
    ds4GyroZ = ((inputBuffer[17] << 8 | inputBuffer[18]) / 16);
    //ds4PrevGyroX = ds4GyroX;
    //ds4PrevGyroY = ds4GyroY;
    //ds4PrevGyroZ = ds4GyroZ;

    ds4AccelX = ((inputBuffer[19] << 8 | inputBuffer[20]) / 16);
    ds4AccelY = ((inputBuffer[21] << 8 | inputBuffer[22]) / 16);
    ds4AccelZ = ((inputBuffer[23] << 8 | inputBuffer[24]) / 16);
    //ds4PrevAccelX = ds4AccelX;
    //ds4PrevAccelY = ds4AccelY;
    //ds4PrevAccelZ = ds4AccelZ;

}

void DS4GetStateBT(hid_device* hidHandle) {

}

void DS4GetState(hid_device* hidHandle) {
    while (ds4IsPresent == false) {
        DS4Enumeration();
    }

    if (ds4IsPresent == true) {
        DS4Presence(hidHandle);
    }

    if (ds4IsPresent == true) {
        if (ds4IsWireless == true) {
            DS4GetStateBT(hidHandle);
        }
        if (ds4IsWireless == false) {
            DS4GetStateUSB(hidHandle);
        }
    }
}

void DS4SetStateUSB(hid_device* hidHandle) {
    memset(outputBuffer, 0, 80);

    outputBuffer[0] = 0x05;
    outputBuffer[1] = 0xFF; // F0 Nothing, F3 Rumble, F7 Rumble + LED
    outputBuffer[2] = 0x04;
    outputBuffer[3] = 0x00;
    outputBuffer[4] = ds4RightMotor; // Right / Weak
    outputBuffer[5] = ds4LeftMotor; // Left / Stong
    outputBuffer[6] = ds4LightRed; // Red
    outputBuffer[7] = ds4LightGreen; // Green
    outputBuffer[8] = ds4LightBlue; // Blue
    outputBuffer[9] = ds4LightOnTime; // LED On Time
    outputBuffer[10] = ds4LightOffTime; // LED Off Time

    hid_set_nonblocking(hidHandle, 0);
    hid_write(hidHandle, outputBuffer, 80);
}

void DS4SetStateBT(hid_device* hidHandle) {

}

void DS4SetState(hid_device* hidHandle) {
    if (ds4IsPresent == true) {
        if (ds4IsWireless == true) {
            DS4SetStateBT(hidHandle);
        }
        if (ds4IsWireless == false) {
            DS4SetStateUSB(hidHandle);
        }
    }
}

void DS4CloseStateUSB(hid_device* hidHandle) {
    memset(outputBuffer, 0, 80);

    outputBuffer[0] = 0x05;
    outputBuffer[1] = 0xF7; // F0 Nothing, F3 Rumble, F7 Rumble + LED
    outputBuffer[2] = 0x04;
    outputBuffer[3] = 0x00;
    outputBuffer[4] = 0x00; // Right / Weak
    outputBuffer[5] = 0x00; // Left / Stong
    outputBuffer[6] = 0x00; // Red
    outputBuffer[7] = 0x00; // Green
    outputBuffer[8] = 0x00; // Blue
    outputBuffer[9] = 0x00; // LED On Time
    outputBuffer[10] = 0xFF; // LED Off Time

    hid_set_nonblocking(hidHandle, 0);
    hid_write(hidHandle, outputBuffer, 80);
}

void DS4CloseStateBT(hid_device* hidHandle) {

}

void DS4CloseState(hid_device* hidHandle) {
    ClearScreen();
    printf("Shutting down handler\n");

    if (ds4IsPresent == true) {
        if (ds4IsWireless == true) {
            DS4CloseStateBT(hidHandle);
        }
        if (ds4IsWireless == false) {
            DS4CloseStateUSB(hidHandle);
        }
    }
    ds4IsPresent == false;

    // Give us a moment to close the device
    Sleep(1000);

    hid_close(hidHandle);
    hid_exit();
}

int main() {

    DS4GetState(hidHandle);
    ClearScreen();

    ds4LightRed = 0;
    ds4LightGreen = 0;
    ds4LightBlue = 255;
    ds4LightOnTime = 255;
    ds4LightOffTime = 0;

    while (keepRunning) {
        UpdateScreen();

        DS4GetState(hidHandle);

        /*printf("%02X %02X %02X %02X %02X %02X %02X %02X\n\n", inputBuffer[0], inputBuffer[1], inputBuffer[2], inputBuffer[3], inputBuffer[4], inputBuffer[5], inputBuffer[6], inputBuffer[7]);
        printf("%02X %02X %02X %02X %02X %02X %02X %02X\n\n", inputBuffer[8], inputBuffer[9], inputBuffer[10], inputBuffer[11], inputBuffer[12], inputBuffer[13], inputBuffer[14], inputBuffer[15]);
        printf("%02X %02X %02X %02X %02X %02X %02X %02X\n\n", inputBuffer[16], inputBuffer[17], inputBuffer[18], inputBuffer[19], inputBuffer[20], inputBuffer[21], inputBuffer[22], inputBuffer[23]);
        printf("%02X %02X %02X %02X %02X %02X %02X %02X\n\n", inputBuffer[24], inputBuffer[25], inputBuffer[26], inputBuffer[27], inputBuffer[28], inputBuffer[29], inputBuffer[30], inputBuffer[31]);
        printf("%02X %02X %02X %02X %02X %02X %02X %02X\n\n", inputBuffer[32], inputBuffer[33], inputBuffer[34], inputBuffer[35], inputBuffer[36], inputBuffer[37], inputBuffer[38], inputBuffer[39]);
        printf("%02X %02X %02X %02X %02X %02X %02X %02X\n\n", inputBuffer[40], inputBuffer[41], inputBuffer[42], inputBuffer[43], inputBuffer[44], inputBuffer[45], inputBuffer[46], inputBuffer[47]);
        printf("%02X %02X %02X %02X %02X %02X %02X %02X\n\n", inputBuffer[48], inputBuffer[49], inputBuffer[50], inputBuffer[51], inputBuffer[52], inputBuffer[53], inputBuffer[54], inputBuffer[55]);
        printf("%02X %02X %02X %02X %02X %02X %02X %02X\n\n", inputBuffer[56], inputBuffer[57], inputBuffer[58], inputBuffer[59], inputBuffer[60], inputBuffer[61], inputBuffer[62], inputBuffer[63]);
        printf("%02X %02X %02X %02X %02X %02X %02X %02X\n\n", inputBuffer[64], inputBuffer[65], inputBuffer[66], inputBuffer[67], inputBuffer[68], inputBuffer[69], inputBuffer[70], inputBuffer[71]);
        printf("%02X %02X %02X %02X %02X %02X %02X %02X\n\n", inputBuffer[72], inputBuffer[73], inputBuffer[74], inputBuffer[75], inputBuffer[76], inputBuffer[77], inputBuffer[78], inputBuffer[79]);
        printf("%02X %02X %02X %02X %02X %02X %02X %02X\n\n", inputBuffer[80], inputBuffer[81], inputBuffer[82], inputBuffer[83], inputBuffer[84], inputBuffer[85], inputBuffer[86], inputBuffer[87]);
        printf("%02X %02X %02X %02X %02X %02X %02X %02X\n\n", inputBuffer[88], inputBuffer[89], inputBuffer[90], inputBuffer[91], inputBuffer[92], inputBuffer[93], inputBuffer[94], inputBuffer[95]);*/

        printf("Sticks:\n");
        printf("|  LX: %3d  |  LY: %3d  |  L3: %01d\n", ds4LeftStickX, ds4LeftStickY, ds4L3);
        printf("|  RX: %3d  |  RY: %3d  |  R3: %01d\n\n", ds4RightStickX, ds4RightStickY, ds4R3);

        printf("Face:\n");
        printf("|  Cross: %01d  |  Circle: %01d  |  Triangle: %01d  |  Square: %01d\n", ds4Cross, ds4Circle, ds4Triangle, ds4Square);
        printf("|     Up: %01d  |    Left: %01d  |      Down: %01d  |   Right: %01d\n\n", ds4DpadUp, ds4DpadLeft, ds4DpadDown, ds4DpadRight);

        printf("Shoulders:\n");
        printf("|  L1: %01d |  L2: %01d |  Pressure: %3d%%\n", ds4L1, ds4L2, (ds4L2Pressure / 2));
        printf("|  R1: %01d |  R2: %01d |  Pressure: %3d%%\n\n", ds4R1, ds4R2, (ds4R2Pressure / 2));

        printf("Special:\n");
        printf("|  Options: %01d  |  Share: %01d  |  Touchpad: %01d  |  PS: %01d\n\n", ds4Options, ds4Share, ds4Touchpad, ds4PS);

        printf("Rumble:\n");
        printf("|  Left Motor: %3d%%  |  Right Motor: %3d%%\n\n", (ds4LeftMotor / 2), (ds4RightMotor / 2));

        printf("Lightbar:\n");
        printf("|      Red: %3d%%  |     Green: %3d%%  |  Blue: %3d%%\n", ds4LightRed, ds4LightGreen, ds4LightBlue);
        printf("|  Time On: %.*fs\n", 2,  (ds4LightOnTime * 0.01f));
        printf("| Time Off: %.*fs\n\n", 2,  (ds4LightOffTime * 0.01f));

        printf("Gyro:\n");
        printf("|  X: %6d  |  Y: %6d  |  Z: %6d\n\n", ds4GyroX, ds4GyroY, ds4GyroZ);

        printf("Acceleration:\n");
        printf("|  X: %6d  |  Y: %6d  |  Z: %6d\n", ds4AccelX, ds4AccelY, ds4AccelZ);
        //printf("     Battery: %02d\n", ds4Battery);

        if (ds4PS && (ds4L2Pressure || ds4R2Pressure)) {
            ds4LeftMotor = ds4L2Pressure;
            ds4RightMotor = ds4R2Pressure;
        }

        else {
            ds4LeftMotor = 0;
            ds4RightMotor = 0;
        }

        if (ds4Options && ds4Share == true) {
            keepRunning = false;
        }

        if (ds4PS && ds4DpadUp == true) {
            if (ds4LightOnTime != 255) {
                ds4LightOnTime = ds4LightOnTime + 1;
            }
        }
        if (ds4PS && ds4DpadDown == true) {
            if (ds4LightOnTime != 0) {
                ds4LightOnTime = ds4LightOnTime - 1;
            }
        }
        if (ds4PS && ds4DpadLeft == true) {
            if (ds4LightOffTime != 0) {
                ds4LightOffTime = ds4LightOffTime - 1;
            }
        }
        if (ds4PS && ds4DpadRight == true) {
            if (ds4LightOffTime != 255) {
                ds4LightOffTime = ds4LightOffTime + 1;
            }
        }

        DS4SetState(hidHandle);

        Sleep(33);
    }

    DS4CloseState(hidHandle);

    printf("Closed handler\n");

    exit;
    return 0;
}
