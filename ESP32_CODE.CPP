/*
===============================================================================
Project : ESP32 Battery Monitoring and Data Logging System
Author  : ALWIN & NADESH
Platform: ESP32 + INA219 + Raspberry Pi

Description:
This program uses an INA219 current/voltage sensor to monitor battery
parameters through the ESP32. The INA219 communicates with the ESP32 via
the I2C interface.

A hardware timer triggers data acquisition every 30 seconds. During each
sampling cycle, the program measures:

    - Battery Current (mA)
    - Battery Voltage (V)
    - Battery Power (mW)
    - Accumulated Capacity (mAh)

Battery capacity is calculated using the Coulomb Counting method by
integrating current over time.

The ESP32 also accepts UART commands from an external device:

    START  -> Begin data transmission
    STOP   -> Stop data transmission
    RESET  -> Reset accumulated capacity value

Based on the measured current, the battery operating state is determined:

    Current < -20 mA  -> CHARGING
    Current >  20 mA  -> DISCHARGING
    Otherwise         -> IDLE

The collected data is transmitted through Serial2 in CSV format to a
Raspberry Pi or other data-logging device.

Output Format:
    Time(s),Current(mA),Voltage(V),Power(mW),Capacity(mAh),State

Example:
    120.0,450.235,12.456,5600.123,125.432,DISCHARGING

Applications:
    - Battery Monitoring Systems
    - State of Charge (SoC) Estimation
    - Energy Consumption Analysis
    - Battery Data Logging
    - Embedded Energy Management Systems

===============================================================================
*/

#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

const float VOLTAGE_SCALE  = 1.0170;
const float VOLTAGE_OFFSET = 0.0;

float capacity_mAh = 0.0;
unsigned long lastIntegrationTime = 0;

// Timer objects
hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
volatile bool sampleFlag = false;

// Control flags
bool sendData = true;

void ARDUINO_ISR_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  sampleFlag = true;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  Wire.begin(21, 22);

  if (!ina219.begin()) {
    Serial.println("INA219 not found");
    while (1);
  }

  Serial.println("ESP32_READY");

  lastIntegrationTime = millis();

  timer = timerBegin(1000000);
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, 3000000, true, 0); // 30 sec
}

void loop() {

  // =========================
  // 1. RECEIVE COMMANDS (UART)
  // =========================
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "START") {
      sendData = true;
      Serial.println("ACK_START");
    }
    else if (cmd == "STOP") {
      sendData = false;
      Serial.println("ACK_STOP");
    }
    else if (cmd == "RESET") {
      capacity_mAh = 0;
      Serial.println("ACK_RESET");
    }
  }

  // =========================
  // 2. TIMER-BASED SAMPLING
  // =========================
  bool doSample = false;

  portENTER_CRITICAL(&timerMux);
  if (sampleFlag) {
    sampleFlag = false;
    doSample = true;
  }
  portEXIT_CRITICAL(&timerMux);

  // =========================
  // 3. SEND DATA
  // =========================
  if (doSample && sendData) {

    unsigned long now = millis();

    float current_mA      = ina219.getCurrent_mA();
    float busVoltage_V    = ina219.getBusVoltage_V();
    float shuntVoltage_mV = ina219.getShuntVoltage_mV();
    float power_mW        = ina219.getPower_mW();

    float correctedBusVoltage_V = (busVoltage_V * VOLTAGE_SCALE) + VOLTAGE_OFFSET;
    float voltage_V = correctedBusVoltage_V + (shuntVoltage_mV / 1000.0);

    float dt_hours = (now - lastIntegrationTime) / 3600000.0;
    lastIntegrationTime = now;

    capacity_mAh += current_mA * dt_hours;

    const char *state;
    if (current_mA < -20)
      state = "CHARGING";
    else if (current_mA > 20)
      state = "DISCHARGING";
    else
      state = "IDLE";

    float time_sec = now / 1000.0;

    // =========================
    // FORMAT FOR PI (IMPORTANT)
    // =========================
    Serial2.print(time_sec,1);
    Serial2.print(",");
    Serial2.print(current_mA, 3);
    Serial2.print(",");
    Serial2.print(voltage_V, 3);
    Serial2.print(",");
    Serial2.print(power_mW, 3);
    Serial2.print(",");
    Serial2.print(capacity_mAh, 3);
    Serial2.print(",");
    Serial2.println(state);
  }
}
