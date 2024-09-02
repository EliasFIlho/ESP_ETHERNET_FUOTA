#pragma once

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#define LED_PIN 2
#define HIGH 1
#define LOW 0


esp_err_t config_led();
void link_up_patter();
void link_dwm_patter();
void got_ip_patter();
