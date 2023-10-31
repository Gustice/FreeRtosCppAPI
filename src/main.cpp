#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <stdio.h>

#include "task.hpp"
using namespace fos;

const int StackSize = 2048;

extern "C" { // This switch allows the ROS C-implementation to find this main
void app_main(void);
}

static bool boolTask1WasCalled = false;
void task1(void *) {
    ESP_LOGI("Test", "subroutinge called");
    boolTask1WasCalled = true;
    while (true) {
        vTaskDelay(100);
    }
}

void app_main() {

    static Task task(task1, "task1");

    while (true) {
        ESP_LOGI("main", "tick");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
