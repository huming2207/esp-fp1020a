/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"

#include "fp1020a.h"

#define TAG "fp_demo"

void app_main()
{
    printf("Hello world!\n");

    ESP_LOGI(TAG, "Initialising FP1020A");
    fp1020a_init();

    int user_id = 1;

    ESP_LOGI(TAG, "Please press your finger, 1/6");
    fp_ack_t ret = fp1020a_add_fp_start(user_id, FP1020A_USER_LEVEL_1);
    ESP_LOGI(TAG, "Returned %d", ret);

    ESP_LOGI(TAG, "Please press your finger, 2/6");
    ret = fp1020a_add_fp_intermediate(user_id, FP1020A_USER_LEVEL_1);
    ESP_LOGI(TAG, "Returned %d", ret);

    ESP_LOGI(TAG, "Please press your finger, 3/6");
    ret = fp1020a_add_fp_intermediate(user_id, FP1020A_USER_LEVEL_1);
    ESP_LOGI(TAG, "Returned %d", ret);

    ESP_LOGI(TAG, "Please press your finger, 4/6");
    ret = fp1020a_add_fp_intermediate(user_id, FP1020A_USER_LEVEL_1);
    ESP_LOGI(TAG, "Returned %d", ret);

    ESP_LOGI(TAG, "Please press your finger, 5/6");
    ret = fp1020a_add_fp_intermediate(user_id, FP1020A_USER_LEVEL_1);
    ESP_LOGI(TAG, "Returned %d", ret);

    ESP_LOGI(TAG, "Please press your finger, 6/6");
    ret = fp1020a_add_fp_end(user_id, FP1020A_USER_LEVEL_1);
    ESP_LOGI(TAG, "Returned %d", ret);

    for(uint8_t idx = 0; idx < 10; idx++) {
        ESP_LOGI(TAG, "Now try your finger, %u/10", idx);
        fp_auth_t *auth = fp1020a_auth_user();
        ESP_LOGI(TAG, "Returned user ID %u, level %d", auth->user_id, auth->level);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }


    ESP_LOGI(TAG, "Deleting all records");
    ret = fp1020a_remove_all_user();
    ESP_LOGI(TAG, "Returned %d", ret);
}
