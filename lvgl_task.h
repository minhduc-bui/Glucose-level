#ifndef LVGL_TASK_H
#define LVGL_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <lvgl.h>

void lvgl_task(void *pvParameters);
void lvgl_init(void);

#ifdef __cplusplus
}
#endif

#endif // LVGL_TASK_H