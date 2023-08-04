/**
 *****************************************************************************************
 *
 * @file user_app.c
 *
 * @brief User function Implementation.
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
#include "user_app.h"
#include "bas_c.h"
#include "dis_c.h"
#include "hrs_c.h"
#include "gr55xx_sys.h"
#include "app_log.h"
#include "ble_scanner.h"
#include "app_error.h"
#include "utility.h"

/*
 * DEFINES
 *****************************************************************************************
 */
#define APP_SCAN_INTERVAL               160      /**< Determines scan interval(in units of 0.625 ms). */
#define APP_SCAN_WINDOW                 80       /**< Determines scan window(in units of 0.625 ms). */
#define APP_SCAN_DURATION               0     /**< Duration of the scanning(in units of 10 ms). */

#define APP_CONN_INTERVAL_MIN           6        /**< Minimum connection interval(in unit of 1.25ms). */
#define APP_CONN_INTERVAL_MAX           24       /**< Maximum connection interval(in unit of 1.25ms). */
#define APP_CONN_SLAVE_LATENCY          0        /**< Slave latency. */
#define APP_CONN_SUP_TIMEOUT            400      /**< Connection supervisory timeout(in unit of 10 ms). */

/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
static uint16_t s_filter_uuid = BLE_ATT_SVC_HEART_RATE;

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
static void ble_scanner_evt_handler(ble_scanner_evt_t *p_evt)
{
    sdk_err_t    error_code;

    switch (p_evt->evt_type)
    {
        case BLE_SCANNER_EVT_CONNECTED:
            error_code = hrs_c_disc_srvc_start(p_evt->param.conn_idx);
            APP_ERROR_CHECK(error_code);
            break;


        case BLE_SCANNER_EVT_FILTER_MATCH:
            APP_LOG_INFO("Found target device.");
            break;

        default:
            break;
    }
}

/**
 *****************************************************************************************
 *@brief Initialize the BLE scan module.
 *****************************************************************************************
 */
static void gap_scan_init(void)
{
    sdk_err_t                 error_code;
    ble_scanner_init_t        scan_init;
    ble_scanner_filter_data_t filter_data;

    scan_init.scan_param.scan_type     = GAP_SCAN_ACTIVE;
    scan_init.scan_param.scan_mode     = GAP_SCAN_OBSERVER_MODE;
    scan_init.scan_param.scan_dup_filt = GAP_SCAN_FILT_DUPLIC_EN;
    scan_init.scan_param.use_whitelist = false;
    scan_init.scan_param.interval      = APP_SCAN_INTERVAL;
    scan_init.scan_param.window        = APP_SCAN_WINDOW;
    scan_init.scan_param.timeout       = APP_SCAN_DURATION;

    scan_init.conn_param.type                = GAP_INIT_TYPE_DIRECT_CONN_EST;
    scan_init.conn_param.interval_min        = APP_CONN_INTERVAL_MIN;
    scan_init.conn_param.interval_max        = APP_CONN_INTERVAL_MAX;
    scan_init.conn_param.slave_latency       = APP_CONN_SLAVE_LATENCY;
    scan_init.conn_param.sup_timeout         = APP_CONN_SUP_TIMEOUT;

    scan_init.connect_auto = true;

    scan_init.err_handler  = NULL;
    scan_init.evt_handler  = ble_scanner_evt_handler;

    filter_data.appearance = BLE_APPEARANCE_GENERIC_HEART_RATE_SENSOR;
    filter_data.svr_uuid.length = 2;
    filter_data.svr_uuid.p_data     = (uint8_t *)&s_filter_uuid;

    ble_scanner_filter_set(BLE_SCANNER_UUID_FILTER | BLE_SCANNER_APPEARANCE_FILTER, &filter_data);

    error_code = ble_scanner_init(&scan_init);
    APP_ERROR_CHECK(error_code);

    ble_scanner_filter_enable(BLE_SCANNER_FILTER_ALL_MATCH);

    error_code = ble_scanner_start();
    APP_ERROR_CHECK(error_code);
}


/**
 *****************************************************************************************
 * @brief Print heart rate measurement value.
 *****************************************************************************************
 */
static void hr_meas_value_print(hrs_c_hr_meas_t *p_hr_meas)
{
    APP_LOG_INFO("Heart Rate Measurement:%d bpm", p_hr_meas->hr_value);

    if (p_hr_meas->is_sensor_contact_detected)
    {
        APP_LOG_INFO("Contact is Detected.");
    }
    else
    {
        APP_LOG_INFO("Contact is NOT Detected.");
    }

    if (p_hr_meas->energy_expended)
    {
        APP_LOG_INFO("Energy Expended:%d kJ", p_hr_meas->energy_expended);
    }

    printf("RR Interval:");
    for (uint8_t i = 0; i < p_hr_meas->rr_intervals_num; i++)
    {
        printf("  %0.2fms", p_hr_meas->rr_intervals[i]);
    }
    printf("\r\n\r\n");
}

/**
 *****************************************************************************************
 * @brief Print sensor location value.
 *****************************************************************************************
 */
static void hr_sensor_loc_print(hrs_c_sensor_loc_t sensor_loc)
{
    switch (sensor_loc)
    {
        case HRS_C_SENS_LOC_OTHER:
            APP_LOG_INFO("Sensor Location: Other.");
            break;

        case HRS_C_SENS_LOC_CHEST:
            APP_LOG_INFO("Sensor Location: Chest.");
            break;

        case HRS_C_SENS_LOC_FINGER:
            APP_LOG_INFO("Sensor Location: Finger.");
            break;

        case HRS_C_SENS_LOC_HAND:
            APP_LOG_INFO("Sensor Location: Hand.");
            break;

        case HRS_C_SENS_LOC_EARLOBE:
            APP_LOG_INFO("Sensor Location: Earlobe.");
            break;

        case HRS_C_SENS_LOC_FOOT:
            APP_LOG_INFO("Sensor Location: Foot.");
            break;

        default:
            break;
    }
}

/**
 *****************************************************************************************
 * @brief Process HRS Client event.
 *****************************************************************************************
 */
static void hrs_c_evt_process(hrs_c_evt_t *p_evt)
{
    sdk_err_t error_code;

    switch (p_evt->evt_type)
    {
        case HRS_C_EVT_DISCOVERY_COMPLETE:
            APP_LOG_INFO("Heart Rate Service discovery completely.");
            error_code = bas_c_disc_srvc_start(p_evt->conn_idx);
            APP_ERROR_CHECK(error_code);
            break;

        case HRS_C_EVT_HR_MEAS_NTF_SET_SUCCESS:
            APP_LOG_INFO("Heart Rate Measurment Notification had been set.");
            break;

        case HRS_C_EVT_HR_MEAS_VAL_RECEIVE:
            hr_meas_value_print(&p_evt->value.hr_meas_buff);
            break;

        case HRS_C_EVT_SENSOR_LOC_READ_RSP:
            hr_sensor_loc_print(p_evt->value.sensor_loc);
            break;

        default:
            break;
    }
}

/**
 *****************************************************************************************
 * @brief Process BAS Client event.
 *****************************************************************************************
 */
static void bas_c_evt_process(bas_c_evt_t *p_evt)
{
    sdk_err_t error_code;

    switch (p_evt->evt_type)
    {
        case BAS_C_EVT_DISCOVERY_COMPLETE:
            APP_LOG_INFO("Battery Service discovery completely.");
            error_code = dis_c_disc_srvc_start(p_evt->conn_idx);
            APP_ERROR_CHECK(error_code);
            break;

        case BAS_C_EVT_BAT_LEVEL_NTF_SET_SUCCESS:
            APP_LOG_INFO("Battery Level Notification had been set.");
            break;

        case BAS_C_EVT_BAT_LEVE_RECEIVE:
            APP_LOG_INFO("Battery Level: %d%%", p_evt->bat_level);
            break;

        default:
            break;
    }
}

/**
 *****************************************************************************************
 * @brief Print system id response.
 *****************************************************************************************
 */
static void system_id_print(dis_c_sys_id_t *p_sys_id)
{
    APP_LOG_INFO("System ID");
    APP_LOG_INFO("Manufacturer Identifier: 0X%02x%02x%02x%02x%02x",
                 p_sys_id->manufacturer_id[4],
                 p_sys_id->manufacturer_id[3],
                 p_sys_id->manufacturer_id[2],
                 p_sys_id->manufacturer_id[1],
                 p_sys_id->manufacturer_id[0]);

    APP_LOG_INFO("Organizationally Unique Identifier:0X%02x%02x%02x",
                 p_sys_id->org_unique_id[2],
                 p_sys_id->org_unique_id[1],
                 p_sys_id->org_unique_id[0]);
}

/**
 *****************************************************************************************
 * @brief Print certification data list response.
 *****************************************************************************************
 */
static void cert_data_print(uint8_t *p_data, uint16_t length)
{
    uint8_t str[DIS_C_STRING_LEN_MAX] = {0};

    APP_LOG_INFO("IEEE 11073-20601 CERT");

    switch (p_data[0])
    {
        case DIS_C_11073_BODY_EMPTY:
            APP_LOG_INFO("EMPT BODY");
            break;

        case DIS_C_11073_BODY_IEEE:
            APP_LOG_INFO("IEEE BODY");
            break;
        case DIS_C_11073_BODY_CONTINUA:
            APP_LOG_INFO("CONTINUA BODY");
            break;

        case DIS_C_11073_BODY_EXP:
            APP_LOG_INFO("EXP BODY");
            break;

        default:
            break;
    }
    memcpy(str, &p_data[2], length - 2);
    APP_LOG_INFO("Authoritative Body Structure:0X%02x", p_data[1]);
    APP_LOG_INFO("Authoritative Body Data:%s", str);
}

/**
 *****************************************************************************************
 * @brief Print pnp id response.
 *****************************************************************************************
 */
static void pnp_id_print(dis_c_pnp_id_t *p_pnp_id)
{
    APP_LOG_INFO("PnP ID");
    APP_LOG_INFO("Vendor ID Source:0X%02x", p_pnp_id->vendor_id_source);
    APP_LOG_INFO("Vendor ID:       0X%04x", p_pnp_id->vendor_id);
    APP_LOG_INFO("Product ID:      0X%04x", p_pnp_id->product_id);
    APP_LOG_INFO("Product Version: 0X%04x", p_pnp_id->product_version);
}

/**
 *****************************************************************************************
 * @brief Print DIS Client read response.
 *****************************************************************************************
 */
static void dis_c_read_rsp_print(ble_dis_c_read_rsp_t *p_char_read_rsp)
{
    uint8_t str[DIS_C_STRING_LEN_MAX] = {0};

    switch (p_char_read_rsp->char_type)
    {
        case DIS_C_SYS_ID:
            system_id_print(&p_char_read_rsp->encode_rst.sys_id);
            break;

        case DIS_C_MODEL_NUM:
            memcpy(str, p_char_read_rsp->encode_rst.string_data.p_data, p_char_read_rsp->encode_rst.string_data.length);
            APP_LOG_INFO("Model Number:      %s", str);
            break;

        case DIS_C_SERIAL_NUM:
            memcpy(str, p_char_read_rsp->encode_rst.string_data.p_data, p_char_read_rsp->encode_rst.string_data.length);
            APP_LOG_INFO("Serial Number:     %s", str);
            break;

        case DIS_C_HW_REV:
            memcpy(str, p_char_read_rsp->encode_rst.string_data.p_data, p_char_read_rsp->encode_rst.string_data.length);
            APP_LOG_INFO("Hardware Revision: %s", str);
            break;

        case DIS_C_FW_REV:
            memcpy(str, p_char_read_rsp->encode_rst.string_data.p_data, p_char_read_rsp->encode_rst.string_data.length);
            APP_LOG_INFO("Firmware Revision: %s", str);
            break;

        case DIS_C_SW_REV:
            memcpy(str, p_char_read_rsp->encode_rst.string_data.p_data, p_char_read_rsp->encode_rst.string_data.length);
            APP_LOG_INFO("Software Revision: %s", str);
            break;

        case DIS_C_MANUF_NAME:
            memcpy(str, p_char_read_rsp->encode_rst.string_data.p_data, p_char_read_rsp->encode_rst.string_data.length);
            APP_LOG_INFO("Manufacturer Name: %s", str);
            break;

        case DIS_C_CERT_LIST:
            cert_data_print(p_char_read_rsp->encode_rst.cert_list.p_list, p_char_read_rsp->encode_rst.cert_list.list_length);
            break;

        case DIS_C_PNP_ID:
            pnp_id_print(&p_char_read_rsp->encode_rst.pnp_id);
            break;

        default:
            break;
    }
}

/**
 *****************************************************************************************
 * @brief Process DIS Client event.
 *****************************************************************************************
 */
static void dis_c_evt_process(dis_c_evt_t *p_evt)
{
    switch (p_evt->evt_type)
    {
        case DIS_C_EVT_DISCOVERY_COMPLETE:
            APP_LOG_INFO("Device Information Service discovery completely.");
            break;

        case DIS_C_EVT_DEV_INFORMATION_READ_RSP:
            dis_c_read_rsp_print(&p_evt->read_rsp);
            break;

        default:
            break;
    }
}

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void app_disconnected_handler(uint8_t conn_idx, const uint8_t disconnect_reason)
{
    sdk_err_t    error_code;

    error_code = ble_scanner_start();
    APP_ERROR_CHECK(error_code);
}

void ble_init_cmp_callback(void)
{
    sdk_err_t     error_code;
    gap_bdaddr_t  bd_addr;
    sdk_version_t version;

    sys_sdk_verison_get(&version);
    APP_LOG_INFO("Goodix GR551x SDK V%d.%d.%d (commit %x)",
                 version.major, version.minor, version.build, version.commit_id);

    error_code = ble_gap_addr_get(&bd_addr);
    APP_ERROR_CHECK(error_code);
    APP_LOG_INFO("Local Board %02X:%02X:%02X:%02X:%02X:%02X.",
                 bd_addr.gap_addr.addr[5],
                 bd_addr.gap_addr.addr[4],
                 bd_addr.gap_addr.addr[3],
                 bd_addr.gap_addr.addr[2],
                 bd_addr.gap_addr.addr[1],
                 bd_addr.gap_addr.addr[0]);
    APP_LOG_INFO("Heart Rate Service Client example started.");

    error_code = bas_client_init(bas_c_evt_process);
    APP_ERROR_CHECK(error_code);

    error_code = dis_client_init(dis_c_evt_process);
    APP_ERROR_CHECK(error_code);

    error_code = hrs_client_init(hrs_c_evt_process);
    APP_ERROR_CHECK(error_code);

    gap_scan_init();
}
