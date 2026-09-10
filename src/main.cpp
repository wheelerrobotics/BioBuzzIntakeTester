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
constexpr float voltageThreshold = 2.5;
const String startUpMessage = "====== SYSTEM STARTED RECORDING ======";
const String modeSwitchedTo1 = "====== MODE SWITCHED TO BREAKTIME REPORT ======";
const String modeSwitchedTo2 = "====== MODE SWITCHED TO BREAK STATUS CHANGE ======";
const String modeSwitchedTo3 = "====== MODE SWITCHED TO RAW TELEMETRY (may impact sampling rate) ======";
const String invalidByteMessage = "====== INVALID BYTECODE READ, VALID BYTES = 1,2,3, SYSTEM UNAFFECTED ======";

// System variables
systemModeENUM systemMode;
unsigned long beamBreakTime = 0;
int lastBeamStatus = LOW;

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
    pinMode(inputPin, INPUT_PULLUP);
    setMode(BREAKTIME_REPORT);
    Serial.begin(115200);
    Serial.println(startUpMessage);
}

void loop() {
    switch (systemMode) {
        case BREAKTIME_REPORT: {
            const int status = digitalRead(inputPin);
            if (status != lastBeamStatus) {
                if (status == HIGH) {
                    Serial.println("Beam broken - Switched at (ms):" + millis());
                    beamBreakTime = millis();
                    lastBeamStatus = status;
                    break;
                }
                unsigned long nowTime = millis();
                unsigned long timeSpentBroken = nowTime - beamBreakTime;
                Serial.println("Beam now intact - Switched at (ms):" + millis());
                Serial.println("!! CSV LOG (dont really work rn) !!:");
                Serial.println("timeSpentBroken,firstBrokenAt,firstIntactAt");
                Serial.println(timeSpentBroken + beamBreakTime + nowTime);
                lastBeamStatus = status;
            }
            break;
        }
        case BREAK_STATUS_CHANGE: {
            const int status = digitalRead(inputPin);
            if (status != lastBeamStatus) {
                if (status == LOW) {
                    Serial.println("Beam intact - Switched at (ms):" + millis());
                } else {
                    Serial.println("Beam broken - Switched at (ms):" + millis());
                }
                lastBeamStatus = status;
            }
            break;
        }
        case RAW_TELEMETRY:
            if (digitalRead(inputPin == LOW)) {
                Serial.println("beam intact");
            } else {
                Serial.println("beam broken");
            }
            break;
    }

    if (!Serial.available()) {
        return;
    }

    int btye = Serial.read();
    switch (btye) {
        case 1:
            setMode(BREAKTIME_REPORT);
            return;
        case 2:
            setMode(BREAK_STATUS_CHANGE);
            return;
        case 3:
            setMode(RAW_TELEMETRY);
            return;
        default:
            Serial.println(invalidByteMessage);
    }
}