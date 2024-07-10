/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "driver/uart.h"
#include "string.h"
#include "lr_if.h"

void app_main(void)
{
    const uart_port_t uart_num = UART_NUM_1;
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
    };
    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(uart_num, 18, 17, 4, 5));

    const int uart_buffer_size = (1024 * 2);
    QueueHandle_t uart_queue;
    // Install UART driver using an event queue here
    ESP_ERROR_CHECK(uart_driver_install(uart_num, uart_buffer_size,
                                        uart_buffer_size, 10, &uart_queue, 0));

    SemaphoreHandle_t mutex = xSemaphoreCreateMutex();
    int data[8] = {0};
    lr_if_init(data, mutex);
    uint8_t MAC[6];
    ESP_ERROR_CHECK(esp_read_mac(MAC, ESP_MAC_WIFI_STA));
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n\r", MAC2STR(MAC));
    while (true)
    {
        vTaskDelay(30000 / portTICK_PERIOD_MS);
        char data_print[128] = {0};
        if (xSemaphoreTake(mutex, 100) == pdPASS)
        {

            memset(data_print, 0, strlen(data_print));
            snprintf(data_print, sizeof(data_print), "%d,%d,%d,%d,%d,%d,%d,%d",
                     data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
            memset(data, 0, 8 * sizeof(int));
            xSemaphoreGive(mutex);
            int ret = uart_write_bytes(uart_num, data_print, strlen(data_print));
            uart_wait_tx_done(uart_num, 100);
            printf("Sended %s\n\r", data_print);
        }
    }
}