#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <lvgl.h>
#include "TAMC_GT911.h"
#include "lvgl_task.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "ui/ui.h"
#include "spo2_task.h"

#define TFT_HOR_RES 320
#define TFT_VER_RES 480
#define TFT_ROTATION LV_DISPLAY_ROTATION_270
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))
static uint32_t draw_buf[DRAW_BUF_SIZE / 4];

static TFT_eSPI tft = TFT_eSPI();
static TAMC_GT911 tp = TAMC_GT911(TOUCH_SDA, TOUCH_SCL, TOUCH_INT, TOUCH_RST, TFT_WIDTH, TFT_HEIGHT);

// Touchpad read callback
static void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    //Disable for dont conflic with SPO2 task
    // tp.read();
    // if (tp.isTouched)
    // {
    //     data->state = LV_INDEV_STATE_PRESSED;
    //     data->point.x = tp.points[0].x;
    //     data->point.y = tp.points[0].y;
    // }
    // else
    // {
    //     data->state = LV_INDEV_STATE_RELEASED;
    // }
}

// Tick callback
static uint32_t my_tick(void)
{
    return millis();
}

// Example event callback
static void event_cb2(lv_event_t *e)
{
    lv_obj_t *target = (lv_obj_t *)lv_event_get_target(e);
    lv_obj_t *cont = (lv_obj_t *)lv_event_get_current_target(e);
    if (target != cont)
    {
        lv_obj_set_style_bg_color(target, lv_palette_main(LV_PALETTE_RED), 0);
    }
}

void lvgl_task(void *pvParameters)
{

    long time = 0;
    long chart_update_time=0;
    while (true)
    {
        lv_task_handler();
        vTaskDelay(pdMS_TO_TICKS(2));
        if(millis() - time > 100)
        {
            time = millis();
            // Update the screen every second
            lv_label_set_text_fmt(objects.lb_spo2, "%d", average_spo2);
            lv_label_set_text_fmt(objects.lb_heartrate, "%d", average_heartRate);
        }
        if(millis() - chart_update_time > 40)
        {
            chart_update_time = millis();
            // Update the chart every second
            lv_chart_set_next_value(objects.chart_raw, series, chart_red);
        }
    }
}
void lvgl_init()
{
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    lv_init();
    lv_tick_set_cb(my_tick);

    lv_display_t *disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, draw_buf, sizeof(draw_buf));
    tft.invertDisplay(true);
    tft.fillScreen(TFT_BLACK);
    lv_display_set_rotation(disp, TFT_ROTATION);

    tp.begin();
    tp.setRotation(ROTATION_NORMAL);
    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_touchpad_read);

    // // Create UI
    // lv_obj_t *label = lv_label_create(lv_screen_active());
    // lv_label_set_text(label, "Hello LVGL RTOS Task");
    // lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    // delay(1000);

    // test button and event callback
    // lv_obj_t *cont = lv_obj_create(lv_screen_active());
    // lv_obj_set_size(cont, 160, 240);
    // lv_obj_align(cont, LV_ALIGN_CENTER, 0, 0);
    // lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW_WRAP);
    // for (uint32_t i = 0; i < 30; i++)
    // {
    //     lv_obj_t *btn = lv_button_create(cont);
    //     lv_obj_set_size(btn, 70, 50);
    //     lv_obj_add_flag(btn, LV_OBJ_FLAG_EVENT_BUBBLE);
    //     lv_obj_t *lb = lv_label_create(btn);
    //     lv_label_set_text_fmt(lb, "%" LV_PRIu32, i);
    //     lv_obj_center(lb);
    // }
    // lv_obj_add_event_cb(cont, event_cb2, LV_EVENT_CLICKED, NULL);
    // Create the LVGL RTOS task

    // Load SPO2 screen
    ui_init();

    xTaskCreatePinnedToCore(
        lvgl_task,
        "lvgl",
        8192,
        NULL,
        1,
        NULL,
        1);
}