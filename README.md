# OTAdrive IDF for ESP32 devices
This is beta version of the library and mau have some problems. Please report us any issues.

## Features

* Check for available update
* Do OTA firmware update
* Sends progress of Updates via the esp_event_loop

## Example
Bellow is an example shows how to use the library.

```c
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
        if (otadrive_timeTick(60))
        {
            otadrive_result r = otadrive_updateFirmwareInfo();
            ESP_LOGI(TAG, "RES %d,%lu", r.code, r.available_size);
            if (r.code == OTADRIVE_NewFirmwareExists)
            {
                ESP_LOGI(TAG, "Lets download new firmware %s,%luBytes. Current firmware is %s",
                         r.available_version, r.available_size, otadrive_currentversion());
                otadrive_updateFirmware();
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

## Configuration
