#ifndef __LR_IF_H__
#define __LR_IF_H__

#include "esp_system.h"
#include <esp_now.h>
#include <esp_wifi.h>
#include <nvs_flash.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_mac.h"
#include "freertos/semphr.h"

int lr_if_init(int *p_esp_now_count, SemaphoreHandle_t p_mutex);

void callback(const uint8_t *mac_addr, const uint8_t *data, int data_len);

#endif