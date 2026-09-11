//
// Created by Emad Muzaffar on 9/9/26.
// Main platformIO project file, this code is uploaded to the Arduino
//

#include <Arduino.h>

enum systemModeENUM {
    BREAKTIME_REPORT,
    BREAK_STATUS_CHANGE,
    RAW_TELEMETRY
};

// Constants
constexpr int inputPin = 2;
constexpr float voltageThreshold = 2.5;
const String startUpMessage = "====== SYSTEM STARTED RECORDING ======";
const String modeSwitchedTo1 = "====== MODE SWITCHED TO BREAKTIME REPORT ======";
const String modeSwitchedTo2 = "====== MODE SWITCHED TO BREAK STATUS CHANGE ======";
const String modeSwitchedTo3 = "====== MODE SWITCHED TO RAW TELEMETRY (may impact sampling rate) ======";
const String setToPlain = "====== NOW OUTPUTTING PlAIN BREAKTIME REPORT ('p' TO CHANGE) ======";
const String setToNONPlain = "====== NOW CSV BREAKTIME REPORT ('p' TO CHANGE) ======";
const String invalidByteMessage = "====== INVALID BYTECODE READ, VALID BYTES = 1,2,3, SYSTEM UNAFFECTED ======";

// System variables
systemModeENUM systemMode;
unsigned long beamBreakTime = 0;
int lastBeamStatus = LOW;
boolean plainMode = true;

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
    Serial.begin(9600);
    delay(1000);

    pinMode(inputPin, INPUT_PULLUP);
    lastBeamStatus = digitalRead(inputPin);

    Serial.println(startUpMessage);
    setMode(BREAKTIME_REPORT);
}

void loop() {
    switch (systemMode) {
        case BREAKTIME_REPORT: {
            const int status = digitalRead(inputPin);
            if (status != lastBeamStatus) {
                if (status == LOW) {
                    if (plainMode == false) {
                        Serial.print("Beam broken - Switched at (ms):");
                        Serial.println(millis());
                    }
                    beamBreakTime = millis();
                    lastBeamStatus = status;
                    break;
                }
                unsigned long nowTime = millis();
                unsigned long timeSpentBroken = nowTime - beamBreakTime;
                if (plainMode == true) {
                    Serial.println(timeSpentBroken);
                    lastBeamStatus = status;
                    break;
                }
                Serial.print("Beam now intact - Switched at (ms):");
                Serial.println(millis());
                Serial.println("!! CSV LOG (dont really work rn) !!:");
                Serial.println("timeSpentBroken,firstBrokenAt,firstIntactAt");
                Serial.print(timeSpentBroken);
                Serial.print(", ");
                Serial.print(beamBreakTime);
                Serial.print(", ");
                Serial.println(nowTime);
                lastBeamStatus = status;
            }
            break;
        }
        case BREAK_STATUS_CHANGE: {
            const int status = digitalRead(inputPin);
            if (status != lastBeamStatus) {
                if (status == HIGH) {
                    Serial.print("Beam intact - Switched at (ms):");
                    Serial.println(millis());
                } else {
                    Serial.print("Beam broken - Switched at (ms):");
                    Serial.println(millis());
                }
                lastBeamStatus = status;
            }
            break;
        }
        case RAW_TELEMETRY:
            if (digitalRead(inputPin) == HIGH) {
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
        case '1':
            setMode(BREAKTIME_REPORT);
            return;
        case '2':
            setMode(BREAK_STATUS_CHANGE);
            return;
        case '3':
            setMode(RAW_TELEMETRY);
            return;
       case 'p':
            if (plainMode == true) {
                plainMode = false;
                Serial.println(setToNONPlain);
                return;
            }
            plainMode = true;
            Serial.println(setToPlain);
            return;
        default:
            Serial.println(invalidByteMessage);
    }
}