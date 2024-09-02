#include "ethernet.h"
#include "led.h"

bool link_down = false;

const char *TAG = "ETH";

esp_err_t w5500_ethernet_init(esp_eth_handle_t *handle)
{
    esp_err_t ret = gpio_install_isr_service(0);
    if (ret != ESP_OK)
    {
        return ret;
    }
    spi_bus_config_t busfc = {
        .miso_io_num = PIN_MISO,
        .mosi_io_num = PIN_MOSI,
        .sclk_io_num = PIN_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };

    ret = spi_bus_initialize(SPI_USED_HOST, &busfc, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK)
    {
        return ret;
    }

    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    phy_config.reset_gpio_num = -1;
    phy_config.phy_addr = -1;
    spi_device_interface_config_t spi_devcfg = {
        .mode = 0,
        .clock_speed_hz = SPI_FREQ,
        .spics_io_num = PIN_CS,
        .queue_size = 10,
    };

    eth_w5500_config_t w5500_config = ETH_W5500_DEFAULT_CONFIG(SPI_USED_HOST, &spi_devcfg);
    w5500_config.int_gpio_num = -1;
    w5500_config.spi_host_id = SPI_USED_HOST;
    w5500_config.poll_period_ms = 5;
    esp_eth_mac_t *mac = esp_eth_mac_new_w5500(&w5500_config, &mac_config);
    esp_eth_phy_t *phy = esp_eth_phy_new_w5500(&phy_config);
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    return esp_eth_driver_install(&eth_config, handle);
}

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
        ESP_LOGI(TAG, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
        link_up_patter();
        link_down = true;
        break;
    case ETHERNET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "Ethernet Link Down");
        link_down = false;
        link_dwm_patter();
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
    const esp_netif_ip_info_t *ip_info = &event->ip_info;

    ESP_LOGI(TAG, "Ethernet Got IP Address");
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    ESP_LOGI(TAG, "ETHIP:" IPSTR, IP2STR(&ip_info->ip));
    ESP_LOGI(TAG, "ETHMASK:" IPSTR, IP2STR(&ip_info->netmask));
    ESP_LOGI(TAG, "ETHGW:" IPSTR, IP2STR(&ip_info->gw));
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    got_ip_patter();
    // xTaskCreate(ota_task, "OTA Monitor", configMINIMAL_STACK_SIZE + 1024, NULL, tskIDLE_PRIORITY + 1, NULL);
}

esp_err_t add_event_functions()
{

    esp_err_t ret = esp_event_loop_create_default();
    if (ret != ESP_OK)
    {
        return ret;
    }
    ret = esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL);
    if (ret != ESP_OK)
    {
        return ret;
    }
    ret = esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL);
    if (ret != ESP_OK)
    {
        return ret;
    }
    return ESP_OK;
}

esp_err_t configure_network_interface(ip_type_t ip_type, esp_eth_handle_t *eth_handle)
{

    esp_netif_config_t iface_config = ESP_NETIF_DEFAULT_ETH();

    esp_netif_t *iface = esp_netif_new(&iface_config);

    esp_err_t ret = esp_netif_attach(iface, esp_eth_new_netif_glue(*eth_handle));
    if (ret != ESP_OK)
    {
        return ret;
    }

    if (ip_type == STATIC_IP)
    {
        esp_netif_ip_info_t ip_info;
        esp_netif_set_ip4_addr(&ip_info.ip, 10, 109, 49, 150);
        esp_netif_set_ip4_addr(&ip_info.gw, 10, 109, 49, 1);
        esp_netif_set_ip4_addr(&ip_info.netmask, 255, 255, 255, 0);
        ret = esp_netif_dhcpc_stop(iface);
        if (ret != ESP_OK)
        {
            return ret;
        }
        ret = esp_netif_set_ip_info(iface, &ip_info);
        if (ret != ESP_OK)
        {
            return ret;
        }
    }
    return ESP_OK;
}