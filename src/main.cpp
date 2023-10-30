#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <stdio.h>

const int StackSize = 2048;

extern "C" { // This switch allows the ROS C-implementation to find this main
void app_main(void);
}

void app_main() {

    while (true) {
        ESP_LOGI("main", "tick");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
