#include "lr_if.h"

int *esp_now_count;
SemaphoreHandle_t mutex;

int lr_if_init(int *p_esp_now_count, SemaphoreHandle_t p_mutex)
{
    esp_now_count = p_esp_now_count;
    mutex = p_mutex;
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    (esp_wifi_set_protocol(WIFI_IF_AP, WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11N | WIFI_PROTOCOL_LR));
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_recv_cb(callback));
    return 0;
}

void callback(const uint8_t *mac_addr, const uint8_t *data, int data_len)
{
    // printf("%c\n\r", data[0]);
    //  printf("%02x:%02x:%02x:%02x:%02x:%02x\n\r", MAC2STR(mac_addr));
    if (xSemaphoreTake(mutex, 100) == pdPASS)
    {
        esp_now_count[data[0] - '1']++;
        xSemaphoreGive(mutex);
    }
}