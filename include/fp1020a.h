/*
 * Copyright 2019, Jackson Ming Hu <huming2207@gmail.com>
 *
 * This library is used for talking with Biovo FP1020A UART fingerprint modules,
 * based on the official Chinese datasheet & STC51 demo code.
 *
 *
 * This library is licensed under MIT License.
 */

#pragma once

#include <stdint.h>

#define FP1020A_ACK_SUCCESS                 0x00        // Success
#define FP1020A_ACK_FAIL                    0x01        // Fail
#define FP1020A_ACK_FULL                    0x04        // Fingerprint data is full
#define FP1020A_ACK_NOUSER                  0x05        // No such user
#define FP1020A_ACK_USER_OCCUPIED           0x06        // User ID already registered
#define FP1020A_ACK_USER_EXIST              0x07        // User exist
#define FP1020A_ACK_TIMEOUT                 0x08        // Timeout
#define FP1020A_ACK_TRY_AGAIN               0x09        // Try again (when no finger is pressed for authentication)
#define FP1020A_ACK_UART_ERROR            -1          // UART timeout

#define FP1020A_CMD_ADD_FINGER_1            0x01        // Add a fingerprint (the first touch)
#define FP1020A_CMD_ADD_FINGER_2            0x02        // Add a fingerprint (the 2nd to 5th touches)
#define FP1020A_CMD_ADD_FINGER_3            0x03        // Add a fingerprint (the last touch)
#define FP1020A_CMD_DELETE_USER             0x04        // Delete a single user
#define FP1020A_CMD_DELETE_ALL              0x05        // Delete all users
#define FP1020A_CMD_USER_AMOUNT             0x09        // Get the user amount
#define FP1020A_CMD_GET_USER_LVL            0x0A        // Get a particular user's permission level (1-3)
#define FP1020A_CMD_COMP_USER_ID            0x0B        // Compare a fingerprint with an user ID
#define FP1020A_CMD_AUTH_USER               0x0C        // Compare a fingerprint to all existing users in the database
#define FP1020A_CMD_SET_BAUD_RATE           0x21        // Set baud rate
#define FP1020A_CMD_HASH_IMG                0x23        // Hash a bitmap image
#define FP1020A_CMD_UPLOAD_IMG              0x24        // Upload fingerprint bitmap image
#define FP1020A_CMD_SET_COMP_LEVEL          0x28        // Set compare level (0-9)
#define FP1020A_CMD_GET_DSP_SN              0x2A        // Get DSP serial number
#define FP1020A_CMD_GET_ONLINE_USERS        0x2B        // Get all logged in users
#define FP1020A_CMD_SLEEP                   0x2C        // Enter sleep mode
#define FP1020A_CMD_ALLOW_REPEAT            0x2D        // Allow repeat mode
#define FP1020A_CMD_SET_AUTH_DELAY          0x2E        // Get/set authentication delay (0-255, 0 for forever)
#define FP1020A_CMD_GET_HASH                0x31        // Get hash with a certain user ID
#define FP1020A_CMD_GET_OP_RECORD           0x39        // Get operation records
#define FP1020A_CMD_CLEAR_OP_RECORD         0x3A        // Clear operation records
#define FP1020A_CMD_ADD_HASH                0x41        // Add a hash to a certain user ID
#define FP1020A_CMD_COMP_HASH_ONE_USER      0x42        // Upload & compare a hash with one single user in the database
#define FP1020A_CMD_COMP_HASH_ALL           0x43        // Upload & compare a hash with all users in the database
#define FP1020A_CMD_COMP_HASH_FINGER        0x44        // Upload & compare a hash with a finger
#define FP1020A_CMD_GET_AVALIABLE_USER_ID   0x47        // Get the next available user ID


typedef int8_t fp_ack_t;

typedef enum {
    FP1020A_USER_LEVEL_FAIL = -1,
    FP1020A_USER_LEVEL_1 = 1,
    FP1020A_USER_LEVEL_2 = 2,
    FP1020A_USER_LEVEL_3 = 3
} fp_user_lvl_t;

typedef struct {
    fp_user_lvl_t level;
    uint16_t user_id;
} fp_auth_t;

typedef enum {
    FP1020A_BAUD_RATE_UNKNOWN = -1,
    FP1020A_BAUD_RATE_9600 = 1,
    FP1020A_BAUD_RATE_19200 = 2,         // Default setting
    FP1020A_BAUD_RATE_38400 = 3,
    FP1020A_BAUD_RATE_57600 = 4,
    FP1020A_BAUD_RATE_115200 = 5
} fp_baud_rate_t;

/**
 * Initialise UART
 */
void fp1020a_init();

/**
 * Put FP1020A to sleep mode
 */
void fp1020a_sleep();

/**
 * Register a fingerprint
 * @param stage one of the FP1020A_CMD_ADD_FINGER_1/2/3
 * @param user_id User ID
 * @param user_lvl User Level
 * @return Fingerprint sensor ACK
 */
fp_ack_t fp1020a_add_fp(uint8_t stage, uint16_t user_id, fp_user_lvl_t user_lvl);

/**
 * Register a fingerprint - start stage
 * @param user_id User ID
 * @param user_lvl User Level
 * @return Fingerprint sensor ACK
 */
fp_ack_t fp1020a_add_fp_start(uint16_t user_id, fp_user_lvl_t user_lvl);

/**
 * Register a fingerprint - intermediate stage (2nd to 5th)
 * @param user_id User ID
 * @param user_lvl User Level
 * @return Fingerprint sensor ACK
 */
fp_ack_t fp1020a_add_fp_intermediate(uint16_t user_id, fp_user_lvl_t user_lvl);

/**
 * Register a fingerprint - end stage
 * @param user_id User ID
 * @param user_lvl User Level
 * @return Fingerprint sensor ACK
 */
fp_ack_t fp1020a_add_fp_end(uint16_t user_id, fp_user_lvl_t user_lvl);

/**
 * Remove one user by User ID
 * @param user_id User ID
 * @return Fingerprint sensor ACK
 */
fp_ack_t fp1020a_remove_one_user(uint16_t user_id);

/**
 * Remove all users
 * @return Fingerprint sensor ACK
 */
fp_ack_t fp1020a_remove_all_user();

/**
 * Get user amount
 * @return -1 if fails; or the user amount (>= 0)
 */
int fp1020a_get_user_amount();

/**
 * Compare a fingerprint currently putting on the sensor with an User ID
 * @param user_id User ID
 * @return Fingerprint sensor ACK
 */
fp_ack_t fp1020a_comp_one_to_one(uint16_t user_id);

/**
 * Compare all the registered users with a a fingerprint that
 * currently putting on the sensor
 * @return Fingerprint sensor ACK
 */
fp_auth_t *fp1020a_auth_user();

/**
 * Get user permission level
 * @param user_id User ID
 * @return User permission level
 */
fp_user_lvl_t fp1020a_get_user_level(uint16_t user_id);

/**
 * Set comparison strictness level
 * @param level 0-9, default is 5
 * @return Fingerprint sensor ACK
 */
fp_ack_t fp1020a_set_comp_level(uint8_t level);

/**
 * Set UART baud rate
 * @param baud_rate New baud rate
 * @return Old baud rate, or fail message if fails
 */
fp_baud_rate_t fp1020a_set_baud_rate(fp_baud_rate_t baud_rate);

/**
 * Get next available user ID
 * @param id_begin Begin ID range
 * @param id_end End ID range
 * @return Next available user ID
 */
int fp1020a_get_next_avail_user_id(uint16_t id_begin, uint16_t id_end);
