#pragma once

#include <Arduino.h>

// Initialize and start the SpO2 RTOS task
void spo2_task_init();
extern int32_t spo2;          // SPO2 value
extern int32_t heartRate;     // heart rate value
extern int32_t chart_red;
extern int average_heartRate; // heart rate value
extern int average_spo2; // heart rate value
extern int count_time;