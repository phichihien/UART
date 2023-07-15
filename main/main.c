#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "string.h"
#define UART_PORT0 UART_NUM_0
#define UART_PORT1 UART_NUM_1

#define BUF_SIZE (1024)

void uart_task(void *pvParameters) {
    uart_port_t uart_port = (uart_port_t)pvParameters;

    // Cấu hình UART
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,

    };
    uart_param_config(uart_port, &uart_config);
    uart_set_pin(uart_port, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(uart_port, BUF_SIZE * 2, 0, 0, NULL, 0);

    // Gửi chuỗi dữ liệu
    const char *data = "Hello world\n";
    uart_write_bytes(uart_port, data, strlen(data));

    // Đọc dữ liệu từ UART0 và gửi sang UART1
    uint8_t *buffer = (uint8_t *)malloc(BUF_SIZE);
    while (1) {
        int len = uart_read_bytes(uart_port, buffer, BUF_SIZE, 20 / portTICK_PERIOD_MS);
        if (len > 0) {
            uart_write_bytes(UART_PORT1, (char *)buffer, len);
        }
    }

    free(buffer);
    vTaskDelete(NULL);
}

void app_main() {
    xTaskCreate(uart_task, "uart_task", 2048, (void *)UART_PORT0, 10, NULL);
    xTaskCreate(uart_task, "uart_task", 2048, (void *)UART_PORT1, 10, NULL);
}
