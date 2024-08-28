#include "esp_event.h"
#include "esp_log.h"
#include "esp_eth.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "esp_log.h"

// Define GPIOs
// #define PIN_MISO 12
// #define PIN_MOSI 13
// #define PIN_SCLK 14
// #define PIN_CS 15
// #define PIN_INT 4



#define PIN_MISO 19
#define PIN_MOSI 23
#define PIN_SCLK 18
#define PIN_CS 5
#define PIN_INT 4



#define SPI_HOST SPI2_HOST

const char *TAG = "ETH";

/** Event handler for Ethernet events */
static void eth_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    uint8_t mac_addr[6] = {0};
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;

    switch (event_id)
    {
    case ETHERNET_EVENT_CONNECTED:
        esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
        ESP_LOGI(TAG, "Ethernet Link Up");
        ESP_LOGI(TAG, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x",
                 mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
        break;
    case ETHERNET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "Ethernet Link Down");
        break;
    case ETHERNET_EVENT_START:
        ESP_LOGI(TAG, "Ethernet Started");
        break;
    case ETHERNET_EVENT_STOP:
        ESP_LOGI(TAG, "Ethernet Stopped");
        break;
    default:
        break;
    }
}

static void got_ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(TAG, "Got IP Address: " IPSTR, IP2STR(&event->ip_info.ip));
}

esp_err_t w5500_ethernet_init(esp_eth_handle_t *handle)
{
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    spi_bus_config_t busfc = {
        .miso_io_num = PIN_MISO,
        .mosi_io_num = PIN_MOSI,
        .sclk_io_num = PIN_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };

    ESP_ERROR_CHECK(spi_bus_initialize(SPI_HOST, &busfc, SPI_DMA_CH_AUTO));
    
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    phy_config.reset_gpio_num = -1;
    phy_config.phy_addr = 1;
    spi_device_interface_config_t spi_devcfg = {
        .mode = 0,
        .clock_speed_hz = 16*1000*1000,
        .spics_io_num = PIN_CS,
        .queue_size = 10,
    };

    eth_w5500_config_t w5500_config = ETH_W5500_DEFAULT_CONFIG(SPI_HOST, &spi_devcfg);
    //w5500_config.int_gpio_num = PIN_INT;
    w5500_config.int_gpio_num = -1;
    w5500_config.spi_host_id = SPI_HOST;
    w5500_config.poll_period_ms = 5;  
    esp_eth_mac_t *mac = esp_eth_mac_new_w5500(&w5500_config, &mac_config);
    esp_eth_phy_t *phy = esp_eth_phy_new_w5500(&phy_config);
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    return esp_eth_driver_install(&eth_config, handle);
}

void app_main(void)
{

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

    esp_eth_handle_t eth_handle = NULL;
    ESP_ERROR_CHECK(w5500_ethernet_init(&eth_handle));

    ESP_ERROR_CHECK(esp_netif_init());

    esp_event_loop_create_default();
    esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL);
    esp_netif_config_t iface_config = ESP_NETIF_DEFAULT_ETH();
    esp_netif_t *iface = esp_netif_new(&iface_config);
    ESP_ERROR_CHECK(esp_netif_attach(iface, esp_eth_new_netif_glue(eth_handle)));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL));
    ESP_ERROR_CHECK(esp_eth_start(eth_handle));
    // Next steps - Test event loops handles and ethernet interface
    // Try do OTA with the ethernet interface
}
