#include "http_ota.h"
void http_event_handle(esp_http_client_event_handle_t evt, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGI("OTA", "Connected");
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGI("OTA", "Version of firmware: %.*s", evt->data_len, (char *)evt->data);
        break;
    case HTTP_EVENT_ERROR:
        ESP_LOGE("OTA", "SOME ERROR IDK HOW: ");
        break;
    default:
        break;
    }
}

void ota_task(void *pvParameters)
{

    esp_http_client_config_t config = {
        .url = "http://10.109.49.78/ota_version",
        .cert_pem = NULL,
        .method = HTTP_METHOD_GET,
        .event_handler = http_event_handle,
        .port = 8000

    };

    esp_http_client_handle_t handle = esp_http_client_init(&config);
    ESP_LOGI("OTA", "HTTP ROUTINE START");
    while (1)
    {

        esp_http_client_perform(handle);

        ESP_LOGE("HTTP OTA: ", "Device without network interface link");

        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}