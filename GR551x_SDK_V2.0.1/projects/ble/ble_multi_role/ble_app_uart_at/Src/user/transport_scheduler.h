/**
 ********************************************************************************
 *
 * @file transport_scheduler.h
 *
 * @brief Header file
 *
 ********************************************************************************
 * @attention
  #####Copyright (c) 2019 GOODIX
  All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of GOODIX nor the names of its contributors may be used
    to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************************
 */

#ifndef __TRANSPORT_SCHEDULER_H__
#define __TRANSPORT_SCHEDULER_H__

#include <stdint.h>
#include <stdbool.h>

/*
 * ENUMERATIONS
 *****************************************************************************************
 */
/**@brief List of transport flags*/
typedef enum
{
    BLE_TX_CPLT = 0x00,
    GUS_TX_NTF_ENABLE,
    BLE_FLOW_CTRL_ENABLE,
    BLE_TX_FLOW_ON,
    BLE_RX_FLOW_ON,

    TRANSPORT_FLAGS_NB
} transport_flag_t;


/*
 * GLOBAL FUNCTION DECLARATION
 *****************************************************************************************
 */
/**
 *****************************************************************************************
 * @brief  Initialize ble transport flags and ring buffers.
 *****************************************************************************************
 */
void transport_ble_init(void);

/**
 *****************************************************************************************
 * @brief  Initialize uart transport flags and ring buffers.
 *****************************************************************************************
 */
void transport_uart_init(void);

/**
 *****************************************************************************************
 * @brief Push data to ble transmit buffer.
 *
 * @param[in] p_data:  Pointer to data.
 * @param[in] length:  Size of data.
 *****************************************************************************************
 */
void uart_to_ble_buff_data_push(uint8_t const *p_data, uint16_t length);

/**
 *****************************************************************************************
 * @brief Push data to uart transmit buffer.
 *
 * @param[in] p_data: Pointer to data.
 * @param[in] length: Size of data.
 *****************************************************************************************
 */
void ble_to_uart_buff_data_push(uint8_t const *p_data, uint16_t length);

/**
 *****************************************************************************************
 * @brief Device transport schedule.
 *****************************************************************************************
 */
void transport_schedule(void);

/**
 *****************************************************************************************
 * @brief Set transport flag.
 *
 * @param[in] falg:   The transport flag.
 * @param[in] en_dis: True or false.
 *****************************************************************************************
 */
void transport_flag_set(transport_flag_t falg, bool en_dis);

/**
 *****************************************************************************************
 * @brief Confirm transport flag state.
 *
 * @param[in] falg: The transport flag.
 *
 * @return The state of transport flag need confirmed.
 *****************************************************************************************
 */
bool transport_flag_cfm(transport_flag_t falg);

/**
 *****************************************************************************************
 * @brief Update current mtu size.
 *
 * @param[in] new_mtu: New mtu size.
 *****************************************************************************************
 */
void update_mtu_size(uint16_t new_mtu);

/**
 *****************************************************************************************
 * @brief Update ble flow control state.
 *****************************************************************************************
 */
void update_ble_flow_ctrl_state(void);

#endif

