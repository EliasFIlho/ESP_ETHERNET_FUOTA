#pragma once

#include "esp_event.h"
#include "esp_eth.h"
#include "driver/spi_master.h"
#include "esp_netif.h"
#include "esp_log.h"

#define SPI_USED_HOST SPI2_HOST
#define PIN_MISO 19
#define PIN_MOSI 23
#define PIN_SCLK 18
#define PIN_CS 5
#define PIN_INT 4
#define SPI_FREQ (16 * 1000 * 1000)

typedef enum
{
    DHCP = 0,
    STATIC_IP
} ip_type_t;

static void eth_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
static void got_ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
esp_err_t configure_network_interface(ip_type_t ip_type, esp_eth_handle_t *eth_handle);
esp_err_t add_event_functions();
esp_err_t w5500_ethernet_init(esp_eth_handle_t *handle);
