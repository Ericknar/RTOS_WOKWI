#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

/* GPIO definitions */
#define LED_TASK_1    (2U)
#define LED_TASK_2    (4U)

static void TaskFast(void *pvParameters)
{
    (void)pvParameters;

    while (1)
    {
        gpio_set_level(LED_TASK_1, 1);

        printf("[TaskFast] Running on core %d\n", xPortGetCoreID());

        vTaskDelay(pdMS_TO_TICKS(100));

        gpio_set_level(LED_TASK_1, 0);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

static void TaskSlow(void *pvParameters)
{
    (void)pvParameters;

    while (1)
    {
        gpio_set_level(LED_TASK_2, 1);

        printf("[TaskSlow] Running on core %d\n", xPortGetCoreID());

        vTaskDelay(pdMS_TO_TICKS(500));

        gpio_set_level(LED_TASK_2, 0);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void app_main(void)
{
    gpio_reset_pin(LED_TASK_1);
    gpio_set_direction(LED_TASK_1, GPIO_MODE_OUTPUT);

    gpio_reset_pin(LED_TASK_2);
    gpio_set_direction(LED_TASK_2, GPIO_MODE_OUTPUT);

    (void)xTaskCreate(
        TaskFast,
        "TaskFast",
        2048,
        NULL,
        2,
        NULL);

    (void)xTaskCreate(
        TaskSlow,
        "TaskSlow",
        2048,
        NULL,
        1,
        NULL);
}