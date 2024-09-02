idf_component_register(SRCS "http_ota.c" "led.c" "ethernet.c" "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES "esp_eth" "esp_netif" "nvs_flash" "esp_http_client" "esp_https_ota")