#include "led.h"

esp_err_t config_led()
{
    gpio_config_t gpio_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pull_down_en = 0,
        .pull_up_en = 0,
        .pin_bit_mask = (1ULL << LED_PIN)};

    return gpio_config(&gpio_conf);
}

void link_up_patter()
{
    gpio_set_level(LED_PIN, HIGH);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(LED_PIN, LOW);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(LED_PIN, HIGH);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(LED_PIN, LOW);
}

void link_dwm_patter()
{
    gpio_set_level(LED_PIN, HIGH);
    vTaskDelay(50 / portTICK_PERIOD_MS);
    gpio_set_level(LED_PIN, LOW);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    gpio_set_level(LED_PIN, HIGH);
    vTaskDelay(50 / portTICK_PERIOD_MS);
    gpio_set_level(LED_PIN, LOW);
}

void got_ip_patter()
{
    gpio_set_level(LED_PIN, HIGH);
}