#include "spo2_task.h"
#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "ui/ui.h"

MAX30105 particleSensor;
#define BUFFER_LENGTH 100  // 100 samples @25sps => 4s buffer
#define BUFFER_READ_LENGTH 25
uint32_t irBuffer[BUFFER_LENGTH];  // infrared LED sensor data
uint32_t redBuffer[BUFFER_LENGTH]; // red LED sensor data

int32_t bufferLength;  // data length
int32_t spo2 = 0;      // SPO2 value
int8_t validSPO2;      // indicator to show if the SPO2 calculation is valid
int32_t heartRate = 0; // heart rate value
int8_t validHeartRate; // indicator to show if the heart rate calculation is valid
int32_t chart_red = 0;
int32_t average_red = 0;
int average_heartRate = 0; // heart rate value
int average_spo2 = 0;      // heart rate value
// lv_label_set_text_fmt(objects.lb_spo2, "%d%%", spo2);
// lv_label_set_text_fmt(objects.lb_heartrate, "%d", heartRate);
int count_time = 0;
int timecount = 0;
static void spo2_task(void *pvParameters)
{
    // Delay at task start to ensure Serial.begin() has completed before using Serial
    vTaskDelay(pdMS_TO_TICKS(100));
    // configure 25Hz sampling interval
    unsigned long lastSampleTime;
    const uint16_t sampleInterval = 40;
    while (true)
    {
        // initialize timer for first buffer fill
        lastSampleTime = millis();
        bufferLength = BUFFER_LENGTH; // buffer length of 100 stores 4 seconds of samples running at 25sps

        // read the first 100 samples, and determine the signal range
        for (uint16_t i = 0; i < bufferLength; )
        {
            if (millis() - lastSampleTime >= sampleInterval)
            {
                lastSampleTime += sampleInterval;
                while (!particleSensor.available()) particleSensor.check();
                redBuffer[i] = particleSensor.getRed(); chart_red = redBuffer[i] / 1000;
                irBuffer[i] = particleSensor.getIR(); particleSensor.nextSample();
                Serial.print(F("red=")); Serial.print(redBuffer[i], DEC);
                Serial.print(F(", ir=")); Serial.println(irBuffer[i], DEC);
                i++;
            }
        }
        average_red = redBuffer[99];

        // calculate heart rate and SpO2 after first 100 samples (first 4 seconds of samples)
        maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

        // Continuously taking samples from MAX30102.  Heart rate and SpO2 are calculated every 1 second
        while (1)
        {
            // dumping the first 25 sets of samples in the memory and shift the last 75 sets of samples to the top
            for (uint16_t i = BUFFER_READ_LENGTH; i < BUFFER_LENGTH; i++)
            {
                redBuffer[i - BUFFER_READ_LENGTH] = redBuffer[i];
                irBuffer[i - BUFFER_READ_LENGTH] = irBuffer[i];
            }

            // take 25 sets of samples before calculating the heart rate.
            // timed sliding sampling at 25Hz
            lastSampleTime = millis();
            for (uint16_t i = BUFFER_LENGTH - BUFFER_READ_LENGTH; i < BUFFER_LENGTH; )
            {
                if (millis() - lastSampleTime >= sampleInterval)
                {
                    lastSampleTime += sampleInterval;
                    while (!particleSensor.available()) particleSensor.check();
                    redBuffer[i] = particleSensor.getRed(); average_red = redBuffer[i] * 0.01 + average_red * 0.99;
                    chart_red = ((int32_t)redBuffer[i] - average_red) / 4 + 500;
                    irBuffer[i] = particleSensor.getIR(); particleSensor.nextSample();
                    // count_time++;
                    // if (millis() - timecount > 1000)
                    // {
                    //     average_spo2 = count_time;
                    //     timecount = millis();
                    //     count_time = 0;
                    // }
                    i++;
                }
            }

            // After gathering 25 new samples recalculate HR and SP02

            maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
            if (validSPO2 == 1)
            {
                average_spo2 = spo2 * 0.3 + average_spo2 * 0.7;
            }else{
                average_spo2 = 100 * 0.3 + average_spo2 * 0.7;
            }
            if (validHeartRate == 1)
            {
                average_heartRate = heartRate * 0.3 + average_heartRate * 0.7;
            }

        }
    }

}

void spo2_task_init()
{

    // Initialize sensor
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) // Use default I2C port, 400kHz speed
    {
        Serial.println(F("MAX30105 was not found. Please check wiring/power."));
        while (1)
            ;
    }
    // Configure sensor for SpO2 mode
    byte ledBrightness = 60; // Options: 0=Off to 255=50mA
    byte sampleAverage = 4;  // averaging 8 samples
    byte ledMode = 2;       // Options: 1=Red only, 2=Red+IR, 3=Red+IR+Green
    int sampleRate = 1600;   // 200sps; effective 25sps (200/8)
    int pulseWidth = 411;    // Options: 69, 118, 215, 411
    int adcRange = 8192;     // Options: 2048, 4096, 8192, 16384

    particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); // Configure sensor with these settings

    xTaskCreatePinnedToCore(
        spo2_task,
        "spo2",
        4096,
        nullptr,
        2,
        nullptr,
        0);
}