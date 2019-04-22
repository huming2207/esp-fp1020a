#include <stdint.h>
#include <stddef.h>
#include <driver/gpio.h>
#include <driver/uart.h>
#include <esp_log.h>

#include "fp1020a.h"

#define TAG "fp1020a"

static fp_ack_t fp1020a_send_cmd(const uint8_t *payload)
{
    uint8_t checksum = 0;
    for(uint8_t curr_idx = 0; curr_idx < 5; curr_idx++) {
        checksum ^= payload[curr_idx];
    }

    uint8_t data[] = { 0xf5, payload[0], payload[1], payload[2], payload[3], payload[4], checksum, 0xf5 };
    uart_write_bytes(UART_NUM_1, (const char *)data, sizeof(data));
}

void fp1020a_init()
{
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
            .baud_rate = CONFIG_FP1020A_BAUD_RATE,
            .data_bits = UART_DATA_8_BITS,
            .parity    = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, CONFIG_FP1020A_TX_PIN, CONFIG_FP1020A_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_1, CONFIG_FP1020A_BUFFER_SIZE, 0, 0, NULL, 0);
}