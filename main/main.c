#include "ethernet.h"
#include "led.h"
#include "http_ota.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    ESP_LOGI("Ethernet FUOTA", "===========================");
    ESP_LOGI("Ethernet FUOTA", " ");
    ESP_LOGI("Ethernet FUOTA", " Version: v1.0 ");
    ESP_LOGI("Ethernet FUOTA", " ");
    ESP_LOGI("Ethernet FUOTA", "===========================");

    uint8_t mac_addr[6] = {0xA0, 0xDD, 0x6C, 0x03, 0x09, 0xC0};

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    else
    {

        ESP_ERROR_CHECK(ret);
    }

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(config_led());

    esp_eth_handle_t eth_handle = NULL;
    ESP_ERROR_CHECK(w5500_ethernet_init(&eth_handle));
    ESP_ERROR_CHECK(add_event_functions());
    ESP_ERROR_CHECK(configure_network_interface(STATIC_IP, &eth_handle));
    ESP_ERROR_CHECK(esp_eth_start(eth_handle));
    ESP_ERROR_CHECK(esp_eth_ioctl(eth_handle, ETH_CMD_S_MAC_ADDR, mac_addr));

    while (true)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
