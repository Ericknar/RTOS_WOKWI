#include <stdio.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define TASK_ITERATIONS    (100000U)
#define TASK_DELAY_MS      (1U)

/* Shared resource */
static volatile uint32_t g_sharedCounter = 0U;

/* Mutex that protects g_sharedCounter */
static SemaphoreHandle_t g_counterMutex = NULL;

static void CounterTask(void *pvParameters)
{
    const char *taskName = (const char *)pvParameters;
    uint32_t i;

    for (i = 0U; i < TASK_ITERATIONS; i++)
    {
        if (xSemaphoreTake(g_counterMutex, portMAX_DELAY) == pdTRUE)
        {
            uint32_t temp = g_sharedCounter;  // 1. Read
            taskYIELD();                       // context switch — mutex lo bloquea
            g_sharedCounter = temp + 1;        // 3. Write
            xSemaphoreGive(g_counterMutex);
        }
    }

    printf("[%s] Finished\n", taskName);
    vTaskDelete(NULL);
}

void app_main(void)
{
    printf("\n");
    printf("=====================================\n");
    printf("  ESP32 Race Condition Demo (FIXED)  \n");
    printf("=====================================\n");
    printf("Expected : %u\n", (TASK_ITERATIONS * 2U));

    g_counterMutex = xSemaphoreCreateMutex();

    if (g_counterMutex == NULL)
    {
        printf("ERROR: Failed to create mutex\n");
        return;
    }

    xTaskCreate(CounterTask, "Task_A", 2048U, (void *)"Task_A", 5U, NULL);
    xTaskCreate(CounterTask, "Task_B", 2048U, (void *)"Task_B", 5U, NULL);

    vTaskDelay(pdMS_TO_TICKS(30000U));

    printf("Actual   : %lu  <-- correct!\n", g_sharedCounter);
}