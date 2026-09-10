//
// Created by Emad Muzaffar on 9/9/26.
// Main platformIO project file, this code is uploaded to the Arduino
//

#include <Arduino.h>

const enum systemModeENUM {
    BREAKTIME_REPORT,
    BREAK_STATUS_CHANGE,
    RAW_TELEMETRY
};

// Constants
constexpr int inputPin = 0;
const String startUpMessage = "====== SYSTEM STARTED RECORDING ======";
const String modeSwitchedTo1 = "====== MODE SWITCHED TO BREAKTIME REPORT ======";
const String modeSwitchedTo2 = "====== MODE SWITCHED TO BREAK STATUS CHANGE ======";
const String modeSwitchedTo3 = "====== MODE SWITCHED TO RAW TELEMETRY (may impact sampling rate) ======";

// Variables
systemModeENUM systemMode;

void setMode(systemModeENUM newMode) {
    switch (newMode) {
        case BREAKTIME_REPORT:
            Serial.println(modeSwitchedTo1);
            break;
        case BREAK_STATUS_CHANGE:
            Serial.println(modeSwitchedTo2);
            break;
        case RAW_TELEMETRY:
            Serial.println(modeSwitchedTo3);
            break;
    }

    systemMode = newMode;

}

void setup() {
    pinMode(inputPin, INPUT);
    setMode(BREAKTIME_REPORT);
    Serial.begin(115200);
    Serial.println(startUpMessage);
}

void loop() {

}