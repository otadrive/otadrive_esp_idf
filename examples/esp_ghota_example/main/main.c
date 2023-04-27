#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "myWiFi.h"

#include <otadrive_esp.h>

#if CONFIG_EXAMPLE_CONNECT_WIFI
#include "esp_wifi.h"
#endif

static const char *TAG = "otadrive_idf_example";

/* Event handler for catching system events */
static void otadrive_event_handler(void *arg, esp_event_base_t event_base,
                                   int32_t event_id, void *event_data)
{
    if (event_base == ESP_HTTPS_OTA_EVENT)
    {
        switch (event_id)
        {
        case ESP_HTTPS_OTA_START:
            ESP_LOGI(TAG, "OTA started");
            break;
        case ESP_HTTPS_OTA_CONNECTED:
            ESP_LOGI(TAG, "Connected to server");
            break;
        case ESP_HTTPS_OTA_GET_IMG_DESC:
            ESP_LOGI(TAG, "Reading Image Description");
            break;
        case ESP_HTTPS_OTA_VERIFY_CHIP_ID:
            ESP_LOGI(TAG, "Verifying chip id of new image: %d", *(esp_chip_id_t *)event_data);
            break;
        case ESP_HTTPS_OTA_DECRYPT_CB:
            ESP_LOGI(TAG, "Callback to decrypt function");
            break;
        case ESP_HTTPS_OTA_WRITE_FLASH:
            ESP_LOGD(TAG, "Writing to flash: %d written", *(int *)event_data);
            break;
        case ESP_HTTPS_OTA_UPDATE_BOOT_PARTITION:
            ESP_LOGI(TAG, "Boot partition updated. Next Partition: %d", *(esp_partition_subtype_t *)event_data);
            break;
        case ESP_HTTPS_OTA_FINISH:
            ESP_LOGI(TAG, "OTA finish");
            break;
        case ESP_HTTPS_OTA_ABORT:
            ESP_LOGI(TAG, "OTA abort");
            break;
        }
    }
}

void otadrive_test(void *pvParameter)
{
    esp_event_handler_register(OTADRIVE_EVENTS, ESP_EVENT_ANY_ID, &otadrive_event_handler, NULL); // Register a handler to get updates on progress
    otadrive_setInfo("bd076abe-a423-4880-85b3-4367d07c8eda", "v@1.2.3.0");

    while (1)
    {
        otadrive_result r = otadrive_updateFirmwareInfo();
        ESP_LOGI(TAG, "RES %d,%lu", r.code, r.available_size);

        otadrive_updateFirmware();
        vTaskDelay(pdMS_TO_TICKS(20000));
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "app_main start");
    // Initialize NVS.
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

#if CONFIG_EXAMPLE_CONNECT_WIFI
    esp_wifi_set_ps(WIFI_PS_NONE);
    ESP_ERROR_CHECK(example_wifi_connect());
#endif // CONFIG_EXAMPLE_CONNECT_WIFI

    xTaskCreate(&otadrive_test, "otadrive_example_task", 1024 * 16, NULL, 5, NULL);
}