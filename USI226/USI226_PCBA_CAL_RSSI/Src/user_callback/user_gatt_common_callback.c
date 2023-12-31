/**
 *****************************************************************************************
 *
 * @file user_gatt_common_callback.c
 *
 * @brief  BLE GATT Callback Function Implementation.
 *
 *****************************************************************************************
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

/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "gr55xx_sys.h"
#include "app_log.h"
#include "user_app.h"

/*
* LOCAL FUNCTION DECLARATION
*****************************************************************************************
*/
static void app_gatt_mtu_exchange_cb(uint8_t conn_idx, uint8_t status, uint16_t mtu);

/*
 * GLOBAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
const gatt_common_cb_fun_t app_gatt_common_callback =
{
    .app_gatt_mtu_exchange_cb = app_gatt_mtu_exchange_cb,
};

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
/**
 *****************************************************************************************
 * @brief This callback function will be called when the mtu has been exchanged.
 *
 * @param[in] conn_idx: The connection index.
 * @param[in] status:   The status of GATT operation.
 * @param[in] mtu:      The value of exchanged mtu.
 *****************************************************************************************
 */
static void app_gatt_mtu_exchange_cb(uint8_t conn_idx, uint8_t status, uint16_t mtu)
{
APP_LOG_DEBUG("<%s mtu:%d>", __func__,mtu);

	if (BLE_SUCCESS == status){
		if(cmplt_BleGattc_mtu_exchange){
			cmplt_BleGattc_mtu_exchange(0, &mtu);
		}
	}
	else{
		if(cmplt_BleGattc_mtu_exchange){
			cmplt_BleGattc_mtu_exchange(-1, NULL);
		}
	}
	cmplt_BleGattc_mtu_exchange = NULL;
	
APP_LOG_DEBUG("</%s>", __func__);
}
