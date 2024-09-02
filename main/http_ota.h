#pragma once

#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_event.h"

void http_event_handle(esp_http_client_event_handle_t evt, esp_event_base_t event_base, int32_t event_id, void *event_data);
void ota_task(void *pvParameters);