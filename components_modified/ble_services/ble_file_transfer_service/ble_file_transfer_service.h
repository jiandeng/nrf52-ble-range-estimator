/**
 * Copyright (c) 2012 - 2017, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/**@file
 *
 * @defgroup ble_fts Nordic File Transfer Service
 * @{
 * @ingroup  ble_sdk_srv
 * @brief    Nordic File Transfer Service implementation.
 *
 * @details The Nordic File Transfer Service is a simple GATT-based service with TX and RX characteristics.
 *          Data received from the peer is passed to the application, and the data received
 *          from the application of this service is sent to the peer as Handle Value
 *          Notifications. This module demonstrates how to implement a custom GATT-based
 *          service and characteristics using the SoftDevice. The service
 *          is used by the application to send and receive ASCII text strings to and from the
 *          peer.
 *
 * @note The application must propagate SoftDevice events to the Nordic File Transfer Service module
 *       by calling the ble_fts_on_ble_evt() function from the ble_stack_handler callback.
 */

#ifndef BLE_FILE_TRANSFER_SERVICE_H__
#define BLE_FILE_TRANSFER_SERVICE_H__

#include "sdk_config.h"
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"
#include "app_util_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__
    #ifdef PACKED
        #undef PACKED
    #endif

    #define PACKED(TYPE) TYPE __attribute__ ((packed))
#endif

#define BLE_FTS_DEF(_name)                                                                          \
        static ble_fts_t _name;                                                                             \
        NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                 \
                             BLE_FTS_BLE_OBSERVER_PRIO,                                                     \
                             ble_fts_on_ble_evt, &_name)


#define BLE_UUID_FTS_SERVICE 0x0001                      /**< The UUID of the Nordic File Transfer Service. */

#define OPCODE_LENGTH 1
#define HANDLE_LENGTH 2

#if defined(NRF_SDH_BLE_GATT_MAX_MTU_SIZE) && (NRF_SDH_BLE_GATT_MAX_MTU_SIZE != 0)
    #define BLE_FTS_MAX_DATA_LEN (NRF_SDH_BLE_GATT_MAX_MTU_SIZE - OPCODE_LENGTH - HANDLE_LENGTH) /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic File Transfer Service module. */
#else
    #define BLE_FTS_MAX_DATA_LEN (NRF_SDH_BLE_GATT_MAX_MTU_SIZE_DEFAULT - OPCODE_LENGTH - HANDLE_LENGTH) /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic File Transfer Service module. */
    #warning NRF_BLE_GATT_MAX_MTU_SIZE is not defined.
#endif

#define BLE_FTS_TX_MAX_CMD_LEN (20)
#define BLE_FTS_RX_MAX_CMD_LEN (20)

typedef enum
{
        BLE_FTS_EVT_TX_CMD_READY,
        BLE_FTS_EVT_TX_DATA_READY,
        BLE_FTS_EVT_TX_DATA_COMPLETE, /**< Event indicating that the central has received something from a peer. */
        BLE_FTS_EVT_RX_CMD_RECEIVED,           /**< Event indicating that the central has received something from a peer. */
        BLE_FTS_EVT_RX_DATA_RECEIVED,          /**< Event indicating that the central has written to peripheral. */
        BLE_FTS_EVT_RX_DATA_COMPLETE,      /**< Event indicating that the central has written to peripheral completely. */
        BLE_FTS_EVT_CONNECTED,                /**< Event indicating that the NUS server has disconnected. */
        BLE_FTS_EVT_DISCONNECTED              /**< Event indicating that the NUS server has disconnected. */
} ble_fts_evt_type_t;

/**@brief Structure containing the NUS event data received from the peer. */
typedef PACKED ( struct
{
        ble_fts_evt_type_t evt_type;
        uint16_t conn_handle;
        uint16_t max_data_len;
        uint8_t * p_data;
        uint16_t data_len;
}) ble_fts_evt_t;

/* Forward declaration of the ble_fts_t type. */
typedef struct ble_fts_s ble_fts_t;

/**@brief Nordic File Transfer Service event handler type. */
typedef void (* ble_fts_evt_handler_t)(ble_fts_t * p_ble_fts, ble_fts_evt_t const * p_evt);

/**@brief Nordic File Transfer Service initialization structure.
 *
 * @details This structure contains the initialization information for the service. The application
 * must fill this structure and pass it to the service using the @ref ble_fts_init
 *          function.
 */
typedef PACKED ( struct
{
        ble_fts_evt_handler_t evt_handler; /**< Event handler to be called for handling received data. */
} ) ble_fts_init_t;

/**@brief Nordic File Transfer Service structure.
 *
 * @details This structure contains status information related to the service.
 */
PACKED ( struct ble_fts_s
{
        uint8_t uuid_type;                            /**< UUID type for Nordic File Transfer Service Base UUID. */
        uint16_t service_handle;                      /**< Handle of Nordic File Transfer Service (as provided by the SoftDevice). */
        ble_gatts_char_handles_t tx_data_handles;          /**< Handles related to the TX characteristic (as provided by the SoftDevice). */
        ble_gatts_char_handles_t rx_data_handles;     /**< Handles related to the RX (Data) characteristic (as provided by the SoftDevice). */
        ble_gatts_char_handles_t tx_cmd_handles;    /**< Handles related to the TX (Image Info) */
        ble_gatts_char_handles_t rx_cmd_handles;          /**< Handles related to the RX (Image Info) characteristic (as provided by the SoftDevice). */
        uint16_t conn_handle;                         /**< Handle of the current connection (as provided by the SoftDevice). BLE_CONN_HANDLE_INVALID if not in a connection. */
        bool tx_data_is_notification_enabled;//tx_data_is_notification_enabled;                 /**< Variable to indicate if the peer has enabled notification of the RX characteristic.*/
        bool tx_cmd_is_notification_enabled;
        ble_fts_evt_handler_t evt_handler;
});

/**@brief Function for initializing the Nordic File Transfer Service.
 *
 * @param[out] p_fts      Nordic File Transfer Service structure. This structure must be supplied
 *                        by the application. It is initialized by this function and will
 *                        later be used to identify this particular service instance.
 * @param[in] p_fts_init  Information needed to initialize the service.
 *
 * @retval NRF_SUCCESS If the service was successfully initialized. Otherwise, an error code is returned.
 * @retval NRF_ERROR_NULL If either of the pointers p_fts or p_fts_init is NULL.
 */
uint32_t ble_fts_init (ble_fts_t * p_fts, const ble_fts_init_t * p_fts_init);

/**@brief Function for handling the Nordic File Transfer Service's BLE events.
 *
 * @details The Nordic File Transfer Service expects the application to call this function each time an
 * event is received from the SoftDevice. This function processes the event if it
 * is relevant and calls the Nordic File Transfer Service event handler of the
 * application if necessary.
 *
 * @param[in] p_fts       Nordic File Transfer Service structure.
 * @param[in] p_ble_evt   Event received from the SoftDevice.
 */
void ble_fts_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);

/**@brief Function for sending a string to the peer.
 *
 * @details This function sends the input string as an RX characteristic notification to the
 *          peer.
 *
 * @param[in] p_fts       Pointer to the Nordic File Transfer Service structure.
 * @param[in] p_string    String to be sent.
 * @param[in] length      Length of the string.
 *
 * @retval NRF_SUCCESS If the string was sent successfully. Otherwise, an error code is returned.
 */
uint32_t ble_fts_tx_data_send(ble_fts_t * p_fts, uint8_t * p_data, uint16_t length);

uint32_t ble_fts_tx_cmd_send(ble_fts_t *p_fts, uint8_t * p_cmd, uint16_t length);

uint32_t ble_fts_tx_data_send_file(ble_fts_t * p_fts, uint8_t * p_data, uint32_t data_length, uint32_t max_packet_length);

uint32_t ble_fts_tx_data_send_file_fragment(ble_fts_t * p_fts, uint8_t * p_data, uint32_t data_length);

bool ble_fts_file_transfer_busy(void);

#ifdef __cplusplus
}
#endif

#endif // BLE_NUS_H__

/** @} */
