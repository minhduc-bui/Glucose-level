#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "lvgl_task.h"
#include "ui/ui.h"
#include "spo2_task.h"
#include <Wire.h>

void setup()
{

  Wire.begin(); // I2C speed
  Wire.setBufferSize(128);
  Serial.begin(115200);
  Serial.println("Starting LVGL Task");
  lvgl_init();
  spo2_task_init();
}

void loop()
{
  // Idle loop
  vTaskDelay(pdMS_TO_TICKS(500));
  // send random data to the screen
  //  lv_label_set_text_fmt(objects.lb_spo2, "%d", random(90, 100));
  //  lv_label_set_text_fmt(objects.lb_heartrate, "%d", random(60, 120));
}
