#include <stdint.h>
#include <stddef.h>
#include <driver/gpio.h>
#include <driver/uart.h>
#include <esp_log.h>

#include "fp1020a.h"

#define TAG "fp1020a"

static esp_err_t fp1020a_send_cmd(const uint8_t *payload)
{
    uint8_t checksum = 0;
    for(uint8_t curr_idx = 0; curr_idx < 5; curr_idx++) {
        checksum ^= payload[curr_idx];
    }

    uint8_t data[] = { 0xf5, payload[0], payload[1], payload[2], payload[3], payload[4], checksum, 0xf5 };
    return uart_write_bytes(UART_NUM_1, (const char *)data, sizeof(data)) > 0 ? ESP_OK : ESP_FAIL;
}

static esp_err_t fp1020a_recv_ack(uint8_t *out, size_t len)
{
    return uart_read_bytes(UART_NUM_1, out, len, pdMS_TO_TICKS(10000)) > 0 ? ESP_OK : ESP_FAIL;
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

void fp1020a_sleep()
{
    uint8_t sleep_cmd[] = { FP1020A_CMD_SLEEP, 0, 0, 0, 0 };
    fp1020a_send_cmd(sleep_cmd);
}

fp_ack_t fp1020a_add_fp(const uint8_t stage, const uint16_t user_id, const fp_user_lvl_t user_lvl)
{
    uint8_t fp_cmd_data[] = {
                stage,
                (user_id >> 8u),            // User ID's MSB
                (user_id & 0xffU),          // User ID's LSB
                user_lvl,
                0
            };

    fp1020a_send_cmd(fp_cmd_data);

    uint8_t fp_add_result[5] = { 0 };
    if(fp1020a_recv_ack(fp_add_result, 5) != ESP_OK) return FP1020A_ACK_UART_ERROR;
    return fp_add_result[3]; // If nothing goes wrong, return the status byte.
}

fp_ack_t fp1020a_add_fp_start(const uint16_t user_id, const fp_user_lvl_t user_lvl)
{
    return fp1020a_add_fp(FP1020A_CMD_ADD_FINGER_1, user_id, user_lvl);
}

fp_ack_t fp1020a_add_fp_intermediate(const uint16_t user_id, const fp_user_lvl_t user_lvl)
{
    return fp1020a_add_fp(FP1020A_CMD_ADD_FINGER_2, user_id, user_lvl);
}

fp_ack_t fp1020a_add_fp_end(const uint16_t user_id, const fp_user_lvl_t user_lvl)
{
    return fp1020a_add_fp(FP1020A_CMD_ADD_FINGER_3, user_id, user_lvl);
}

fp_ack_t fp1020a_remove_one_user(const uint16_t user_id)
{
    uint8_t fp_cmd_data[] = {
            FP1020A_CMD_DELETE_USER,
            (user_id >> 8u),            // User ID's MSB
            (user_id & 0xffU),          // User ID's LSB
            0,
            0
    };

    fp1020a_send_cmd(fp_cmd_data);

    uint8_t fp_add_result[5] = { 0 };
    if(fp1020a_recv_ack(fp_add_result, 5) != ESP_OK) return FP1020A_ACK_UART_ERROR;
    return fp_add_result[3]; // If nothing goes wrong, return the status byte.
}

fp_ack_t fp1020a_remove_all_user()
{
    uint8_t fp_cmd_data[] = { FP1020A_CMD_DELETE_ALL, 0, 0, 0, 0 };

    fp1020a_send_cmd(fp_cmd_data);

    uint8_t fp_add_result[5] = { 0 };
    if(fp1020a_recv_ack(fp_add_result, 5) != ESP_OK) return FP1020A_ACK_UART_ERROR;
    return fp_add_result[3]; // If nothing goes wrong, return the status byte.
}

int fp1020a_get_user_amount()
{
    uint8_t fp_cmd_data[] = { FP1020A_CMD_USER_AMOUNT, 0, 0, 0, 0 };

    fp1020a_send_cmd(fp_cmd_data);

    uint8_t fp_add_result[5] = { 0 };
    if(fp1020a_recv_ack(fp_add_result, 5) != ESP_OK) return FP1020A_ACK_UART_ERROR;

    uint16_t user_count = ((uint16_t)(fp_add_result[1] << 8u) | fp_add_result[2]);
    return user_count;
}

fp_ack_t fp1020a_comp_one_to_one(const uint16_t user_id)
{
    uint8_t fp_cmd_data[] = {
            FP1020A_CMD_COMP_USER_ID,
            (user_id >> 8u),            // User ID's MSB
            (user_id & 0xffU),          // User ID's LSB
            0,
            0
    };

    fp1020a_send_cmd(fp_cmd_data);

    uint8_t fp_add_result[5] = { 0 };
    if(fp1020a_recv_ack(fp_add_result, 5) != ESP_OK) return FP1020A_ACK_UART_ERROR;
    return fp_add_result[3]; // If nothing goes wrong, return the status byte.
}

fp_auth_t *fp1020a_auth_user()
{
    uint8_t fp_cmd_data[] = { FP1020A_CMD_AUTH_USER, 0, 0, 0, 0 };

    fp1020a_send_cmd(fp_cmd_data);

    uint8_t fp_add_result[5] = { 0 };
    if(fp1020a_recv_ack(fp_add_result, 5) != ESP_OK) return NULL;

    fp_auth_t *fp_auth = malloc(sizeof(fp_auth_t));

    if(fp_add_result[3] == FP1020A_ACK_NOUSER || fp_add_result[3] == FP1020A_ACK_TIMEOUT) {
        fp_auth->level = FP1020A_USER_LEVEL_FAIL;
        fp_auth->user_id = 0;
    } else {
        fp_auth->level = (fp_user_lvl_t)fp_add_result[3];
        fp_auth->user_id = ((uint16_t)(fp_add_result[1] << 8u) | fp_add_result[2]);
    }

    return fp_auth;
}

fp_user_lvl_t fp1020a_get_user_level(const uint16_t user_id)
{
    uint8_t fp_cmd_data[] = {
            FP1020A_CMD_GET_USER_LVL,
            (user_id >> 8u),            // User ID's MSB
            (user_id & 0xffU),          // User ID's LSB
            0,
            0
    };

    fp1020a_send_cmd(fp_cmd_data);

    uint8_t fp_add_result[5] = { 0 };
    if(fp1020a_recv_ack(fp_add_result, 5) != ESP_OK || fp_add_result[3] == FP1020A_ACK_NOUSER)
        return FP1020A_USER_LEVEL_FAIL;

    return (fp_user_lvl_t)fp_add_result[3];
}

fp_ack_t fp1020a_set_comp_level(uint8_t level)
{
    uint8_t fp_cmd_data[] = {
            FP1020A_CMD_SET_COMP_LEVEL,
            0,
            level,      // Compare level
            1,          // 1 for writing settings
            0
    };

    fp1020a_send_cmd(fp_cmd_data);

    uint8_t fp_add_result[5] = { 0 };
    if(fp1020a_recv_ack(fp_add_result, 5) != ESP_OK) return FP1020A_ACK_UART_ERROR;
    return (fp_ack_t)fp_add_result[3];
}

fp_baud_rate_t fp1020a_set_baud_rate(fp_baud_rate_t baud_rate)
{
    uint8_t fp_cmd_data[] = {
            FP1020A_CMD_SET_BAUD_RATE,
            0,
            0,
            baud_rate,
            0
    };

    fp1020a_send_cmd(fp_cmd_data);

    uint8_t fp_add_result[5] = { 0 };
    if(fp1020a_recv_ack(fp_add_result, 5) != ESP_OK) return FP1020A_BAUD_RATE_UNKNOWN;
    return (fp_baud_rate_t)fp_add_result[3];
}

int fp1020a_get_next_avail_user_id(const uint16_t id_begin, const uint16_t id_end)
{
    uint8_t fp_cmd_data[] = {
            FP1020A_CMD_GET_AVALIABLE_USER_ID,
            (id_begin >> 8u),
            (id_begin & 0xffU),
            (id_end >> 8u),
            (id_end & 0xffU)
    };

    fp1020a_send_cmd(fp_cmd_data);

    uint8_t fp_add_result[5] = { 0 };
    if(fp1020a_recv_ack(fp_add_result, 5) != ESP_OK) return -1;
    if(fp_add_result[3] == FP1020A_ACK_NOUSER || fp_add_result[3] == FP1020A_ACK_FAIL) return -2;
    return ((uint16_t)(fp_add_result[1] << 8u) | fp_add_result[2]);
}