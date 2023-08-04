/**
 *****************************************************************************************
 *
 * @file test226.c
 *
 * @brief 
 *
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "test226.h"
#include "test226_cmd.h"
#include "string.h"
#include "app_log.h"
#include "app_error.h"

#include "test226_info.h"
#include "test226_batt.h"
#include "test226_user.h"
#include "test226_rssi.h"
#include "build_services_proc.h"

#include "thsBoard.h"
#include "user_app.h"

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
 static u8 oldMAC[6] = {0};
 
static uint8_t test226_xorcheck(const uint8_t *buf, uint8_t len);

// completed callback for build services
static void buildSrvs_cmplt_str(s32 rslt, void* argv);
static void buildSrvs_cmplt_hex(s32 rslt, void* argv);
//completed callback for info.read.sn
static void infoReadSN_cmplt_str(s32 rslt, void* argv);
static void infoReadSN_cmplt_hex(s32 rslt, void* argv);
// completed callback for fetchmac
static void fetchMAC_cmplt_str(s32 rslt, void* argv);
static void fetchMAC_cmplt_hex(s32 rslt, void* argv);
// completed callback for pen.cap
static void battCap_cmplt_str(s32 rslt, void* argv);
static void battCap_cmplt_hex(s32 rslt, void* argv);
// completed callback for pen.volt
static void battVolt_cmplt_str(s32 rslt, void* argv);
static void battVolt_cmplt_hex(s32 rslt, void* argv);
// completed callback for pen.temp
static void temp_cmplt_str(s32 rslt, void* argv);
static void temp_cmplt_hex(s32 rslt, void* argv);
// completed callback for pen.selftest
static void selfTest_cmplt_str(s32 rslt, void* argv);
static void selfTest_cmplt_hex(s32 rslt, void* argv);
// completed callback for pen.gcal %g
static void gCal_cmplt_str(s32 rslt, void* argv);
static void gCal_cmplt_hex(s32 rslt, void* argv);
// completed callback for pen.sleep
static void sleep_cmplt_str(s32 rslt, void* argv);
static void sleep_cmplt_hex(s32 rslt, void* argv);
// completed callback for pen.vib %op
static void vib_cmplt_str(s32 rslt, void* argv);
static void vib_cmplt_hex(s32 rslt, void* argv);
// completed callback for pen.rssi %op
static void rssi_cmplt_str(s32 rslt, void* argv);
static void rssi_cmplt_hex(s32 rslt, void* argv);
// completed callback for pen.disconnect
static void disconnect_cmplt_str(s32 rslt, void* argv);
static void disconnect_cmplt_hex(s32 rslt, void* argv);

// completed callback for shipmode
static void shipmode_cmplt_hex(s32 rslt, void* argv);

// completed callback for read_fw
static void read_fw_cmplt_hex(s32 rslt, void* argv);

static void srv1char1_res(s32 rslt, void* argv);

/**
 *****************************************************************************************
 * @brief 
 *
 * @param[in]
 *
 * @return 
 *****************************************************************************************
 */

u8 test226_cmd(const uint8_t* cmd, u8 len, XPrint xprint){
APP_LOG_DEBUG("<%s>", __func__);
    char* CMD = (char*)cmd;
    uint8_t checkCode,xCmd[32],i;
    uint16_t wCmd,wLen;
    s32 tmp32, hexChckRslt, x[32];

    hexChckRslt = hexCmdCheck(cmd,len,&checkCode,&wCmd,&wLen);
//    print("hexChckRslt:%d\n", hexChckRslt);
    
    if(hexChckRslt == 0){
        if(wCmd == PRODUCTION_CLI_READ_MAC){
            cps4041.start_getMAC(&cps4041.rsrc, fetchMAC_cmplt_hex,1);
        }
        else if(wCmd == PRODUCTION_CLI_CONNECT){
            buildServicesProc(buildSrvs_cmplt_hex);
        }
        else if(wCmd == PRODUCTION_CLI_DISCONNECT){
            memcpy(oldMAC,g_loadedMAC,6);
            xBleGap_disconnect(disconnect_cmplt_hex);
        }
        else if(wCmd == PRODUCTION_CLI_READ_RSSI){
            proc_RSSI(1000, rssi_cmplt_hex);
        }
        else if(wCmd == PRODUCTION_CLI_READ_SN){
            proc_ReadSN(infoReadSN_cmplt_hex);
        }
        else if(wCmd == PRODUCTION_CLI_READ_BATT_CAP){
            proc_ReadCap(battCap_cmplt_hex);
        }
        else if(wCmd == PRODUCTION_CLI_RD_BATT_VOLT){
            proc_reqBattVolt(1000, battVolt_cmplt_hex);
        }
        else if(wCmd == PRODUCTION_CLI_NTC_TEMP){
            proc_reqTemp(1000, temp_cmplt_hex);
        }
        else if(wCmd == PRODUCTION_CLI_PCBA_TEST){
            proc_reqFlags(3000, selfTest_cmplt_hex);
        }
        else if(wCmd == PRODUCTION_CLI_PRESSURE_CALI){
            tmp32 = 0;
            tmp32 = cmd[6];        tmp32 <<= 8;
            tmp32 |= cmd[7];
            proc_reqStartGCal(tmp32, 5000, gCal_cmplt_hex);
        }
        else if(wCmd == PRODUCTION_CLI_VIBRATE){
            proc_vibrate(cmd[5], 1000, vib_cmplt_hex);
        }
        else if(wCmd == PRODUCTION_CLI_PENCIL_SLEEP){
            proc_reqEnterSleep(1000, sleep_cmplt_hex);
        }
        else if(wCmd == PRODUCTION_CLI_CHARGER_EN){
            cps4041.charger_en(&cps4041.rsrc);
            pen_rsp(OPERATION_SUCCESS, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_CHARGER_EN, NULL, 0);
        }
        else if(wCmd == PRODUCTION_CLI_CHARGER_DIS){
            cps4041.charger_dis(&cps4041.rsrc);
            pen_rsp(OPERATION_SUCCESS, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_CHARGER_DIS, NULL, 0);
        }
        else if(wCmd == PRODUCTION_CLI_SHIPMODE){
            proc_shipmode(1000, shipmode_cmplt_hex);
        }
        else if(wCmd == PRODUCTION_CLI_READ_FW){
            proc_readFW(1000, read_fw_cmplt_hex);
        }
        else if(wCmd == PRODUCTION_CLI_RESET){
            pen_rsp(OPERATION_SUCCESS, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_RESET, NULL, 0);
            NVIC_SystemReset();
        }
        return 1;
    }
    else if(strncmp(CMD, "pen.debug", strlen("pen.debug"))==0){
        print("+msg@pen.debug(g_loaded:%d, g_linked:%d)\r\n", g_loaded, g_linked);
    }
    else if(strncmp(CMD, "pen.reset", strlen("pen.reset"))==0){
        print("+ok@pen.reset()\r\n");
        NVIC_SystemReset();
    }    
    else if(strncmp(CMD, "pen.mac", strlen("pen.mac"))==0){
        cps4041.start_getMAC(&cps4041.rsrc, fetchMAC_cmplt_str, 1);
        return 1;
    }
    else if(sscanf(CMD, "pen.connect %x %x %x %x %x %x",&x[0],&x[1],&x[2],&x[3],&x[4],&x[5])==6){
        buildServicesProc(buildSrvs_cmplt_str);
        return 1;
    }
    else if(strncmp(CMD, "pen.connect", strlen("pen.connect"))==0){
//        if(g_loaded == 0){
//            print("+err@pen.connect('invalid_mac')\r\n");
//            return 1;
//        }
        buildServicesProc(buildSrvs_cmplt_str);
        return 1;
    }
    else if(strncmp(CMD, "pen.disconnect", strlen("pen.disconnect"))==0){
        memcpy(oldMAC,g_loadedMAC,6);
        xBleGap_disconnect(disconnect_cmplt_str);
        return 1;
    }
    else if(strncmp(CMD, "pen.sn", strlen("pen.sn"))==0){
        proc_ReadSN(infoReadSN_cmplt_str);    // string type, in 1000ms timeout
        return 1;
    }
    else if(strncmp(CMD, "pen.cap", strlen("pen.cap"))==0){
        proc_ReadCap(battCap_cmplt_str);    // string type, in 1000ms timeout
        return 1;
    }
    else if(strncmp(CMD, "pen.volt", strlen("pen.volt"))==0){
        proc_reqBattVolt(1000, battVolt_cmplt_str);
        return 1;
    }
    else if(strncmp(CMD, "pen.temp", strlen("pen.temp"))==0){
        proc_reqTemp(1000, temp_cmplt_str);
        return 1;
    }
    else if(sscanf(CMD, "pen.gcal %d", &tmp32)==1){
        proc_reqStartGCal(tmp32, 5000, gCal_cmplt_str);
        return 1;
    }
    else if(sscanf(CMD, "pen.vib %d", &tmp32)==1){
        proc_vibrate(tmp32, 1000, vib_cmplt_str);
        return 1;
    }
    else if(strncmp(CMD, "pen.selftest", strlen("pen.selftest"))==0){
        proc_reqFlags(3000, selfTest_cmplt_str);
        return 1;
    }
    else if(strncmp(CMD, "pen.sleep", strlen("pen.sleep"))==0){
        proc_reqEnterSleep(1000, sleep_cmplt_str);
        return 1;
    }
    else if(strncmp(CMD, "pen.rssi", strlen("pen.rssi"))==0){
        proc_RSSI(1000, rssi_cmplt_str);
        return 1;
    }
    else if(sscanf(CMD, "charger %d", &tmp32)==1){
        if(tmp32==0){    cps4041.charger_dis(&cps4041.rsrc);    }
        else{    cps4041.charger_en(&cps4041.rsrc);}
        print("+ok@charger(%d)\r\n", tmp32);
        return 1;
    }    
    else if(sscanf(CMD, "srv1ch1.req %x %x %x %x %x %x %x %x ",&x[0],&x[1],&x[2],&x[3],&x[4],&x[5],&x[6],&x[7])==8){
        for(i=0;i<8;i++){    xCmd[i] = x[i];   }
        proc_srv1char1_req(xCmd, 8, srv1char1_res);
        return 1;
    }
    else if(sscanf(CMD, "srv1ch1.req %x %x %x %x %x %x %x ",&x[0],&x[1],&x[2],&x[3],&x[4],&x[5],&x[6])==7){
        for(i=0;i<7;i++){    xCmd[i] = x[i];   }
        proc_srv1char1_req(xCmd, 7, srv1char1_res);
        return 1;
    }
    else if(sscanf(CMD, "srv1ch1.req %x %x %x %x %x %x ",&x[0],&x[1],&x[2],&x[3],&x[4],&x[5])==6){
        for(i=0;i<6;i++){    xCmd[i] = x[i];   }
        proc_srv1char1_req(xCmd, 6, srv1char1_res);
        return 1;
    }
    else if(sscanf(CMD, "srv1ch1.req %x %x %x %x %x ",&x[0],&x[1],&x[2],&x[3],&x[4])==5){
        for(i=0;i<5;i++){    xCmd[i] = x[i];   }
        proc_srv1char1_req(xCmd, 5, srv1char1_res);
        return 1;
    }
    else if(sscanf(CMD, "srv1ch1.req %x %x %x %x ",&x[0],&x[1],&x[2],&x[3])==4){
        for(i=0;i<4;i++){    xCmd[i] = x[i];   }
        proc_srv1char1_req(xCmd, 4, srv1char1_res);
        return 1;
    }
    else if(sscanf(CMD, "srv1ch1.req %x %x %x ",&x[0],&x[1],&x[2])==3){
        for(i=0;i<3;i++){    xCmd[i] = x[i];   }
        proc_srv1char1_req(xCmd, 3, srv1char1_res);
        return 1;
    }
    
    else if((CMD[0]==PRODUCTION_TEST_CLI_HEAD) && (hexChckRslt == -1)){
        print("head should be '0x%02x'\r\n", PRODUCTION_TEST_CLI_HEAD);
        return 1;
    }
    else if((CMD[0]==PRODUCTION_TEST_CLI_HEAD) && (hexChckRslt == -2)){
        print("checkcode should be '0x%02x'\r\n", checkCode);
        return 1;
    }

APP_LOG_DEBUG("</%s> ", __func__);
    return 0;
}

static void infoReadSN_cmplt_str(s32 rslt, void* argv){
APP_LOG_DEBUG("<%s rslt:%d> ", __func__, rslt);
    buff_t* x=(buff_t*)argv;
    if(rslt==0){    print("+ok@pen.sn('%s')\r\n", (char*)x->buff);    }
    else{    print("+err@pen.sn(%d)\r\n", rslt);    }
APP_LOG_DEBUG("</%s> ", __func__);
}
static void infoReadSN_cmplt_hex(s32 rslt, void* argv){
    if(rslt==0){
        buff_t* x = (buff_t*)argv;
        pen_rsp(OPERATION_SUCCESS, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_READ_SN, x->buff, x->len);
    }
    else{
        print("<%s rslt:%d> ", __func__, rslt);
        pen_rsp(OPERATION_FAIL, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_READ_SN, NULL, 0);    
    }
}

static void fetchMAC_cmplt_str(s32 rslt, void* argv){
APP_LOG_DEBUG("<%s rslt:%d> ", __func__, rslt);
    cps4041.cbRegFetched(&cps4041.rsrc, NULL);
    if(rslt==0){
        u8* mac = (u8*)argv;
        print("+ok@pen.mac('%02x:%02x:%02x:%02x:%02x:%02x')\r\n",
        mac[5],
        mac[4],
        mac[3],
        mac[2],
        mac[1],
        mac[0]);
    }
    else{    
        print("+err@pen.mac(%d)\r\n", rslt);
    }
APP_LOG_DEBUG("</%s> ", __func__);
}
static void fetchMAC_cmplt_hex(s32 rslt, void* argv){
    cps4041.cbRegFetched(&cps4041.rsrc, NULL);
    if(rslt==0){
        u8* mac = (u8*)argv;
        pen_rsp(OPERATION_SUCCESS, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_READ_MAC, mac, 6);
    }
    else{    
        pen_rsp(OPERATION_FAIL, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_READ_MAC, NULL, 0);    
    }
}

static void buildSrvs_cmplt_str(s32 rslt, void* argv){
APP_LOG_DEBUG("<%s rslt:%d> ", __func__, rslt);
    if(rslt==0){
        u8* m = argv;
        print("+ok@pen.connect('%02x:%02x:%02x:%02x:%02x:%02x')\r\n",
            m[5],m[4],m[3],m[2],m[1],m[0]
        );
    }
    else{
        print("<%s rslt:%d> ", __func__, rslt);
        print("+err@pen.connect()\r\n");
    }
APP_LOG_DEBUG("</%s> ", __func__);
}
static void buildSrvs_cmplt_hex(s32 rslt, void* argv){
    if(rslt==0){
        pen_rsp(OPERATION_SUCCESS, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_CONNECT, (u8*)argv, 6);
    }
    else{
        print("<%s rslt:%d> ", __func__, rslt);
        pen_rsp(OPERATION_FAIL, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_CONNECT, NULL, 0);
    }
}

static void battCap_cmplt_str(s32 rslt, void* argv){
APP_LOG_DEBUG("<%s rslt:%d> ", __func__, rslt);
    if(rslt==0){    print("+ok@batt.cap(%d)\r\n", *(u8*)argv);    }
    else{    print("+err@batt.cap(%d)\r\n", rslt);    }
APP_LOG_DEBUG("</%s> ", __func__);
}
static void battCap_cmplt_hex(s32 rslt, void* argv){
    if(rslt==0){
        pen_rsp(OPERATION_SUCCESS, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_READ_BATT_CAP, (u8*)argv, 1);
    }
    else{    
        pen_rsp(OPERATION_FAIL, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_READ_BATT_CAP, NULL, 0);    
    }
}

// completed callback for pen.volt
static void battVolt_cmplt_str(s32 rslt, void* argv){
APP_LOG_DEBUG("<%s rslt:%d> ", __func__, rslt);
    if(rslt==0){    print("+ok@pen.volt(%d)\r\n", *(u16*)argv);    }
    else{    print("+err@pen.volt(%d)\r\n", rslt);    }
APP_LOG_DEBUG("</%s> ", __func__);
}
static void battVolt_cmplt_hex(s32 rslt, void* argv){
    if(rslt==0){
        u16 volt = *(u16*)argv;
        u8 tmp[2];
        tmp[0] = volt&0xff;    volt >>= 8;
        tmp[1] = volt&0xff;
        pen_rsp(OPERATION_SUCCESS, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_RD_BATT_VOLT, tmp, 2);
    }
    else{    
        pen_rsp(OPERATION_FAIL, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_RD_BATT_VOLT, NULL, 0);
    }
}

// completed callback for pen.temp
static void temp_cmplt_str(s32 rslt, void* argv){
APP_LOG_DEBUG("<%s rslt:%d> ", __func__, rslt);
    if(rslt==0){    print("+ok@pen.temp(%d)\r\n", *(u8*)argv);    }
    else{    print("+err@pen.temp(%d)\r\n", rslt);    }
APP_LOG_DEBUG("</%s> ", __func__);
}
static void temp_cmplt_hex(s32 rslt, void* argv){
    if(rslt==0){
        pen_rsp(OPERATION_SUCCESS, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_NTC_TEMP, (u8*)argv, 1);
    }
    else{    
        pen_rsp(OPERATION_FAIL, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_NTC_TEMP, NULL, 0);
    }
}

// completed callback for pen.selftest
static void selfTest_cmplt_str(s32 rslt, void* argv){
APP_LOG_DEBUG("<%s rslt:%d> ", __func__, rslt);
    if(rslt==0){    print("+ok@pen.selftest(0x%08x)\r\n", *(u32*)argv);    }
    else{    print("+err@pen.selftest(%d)\r\n", rslt);    }
APP_LOG_DEBUG("</%s> ", __func__);
}
static void selfTest_cmplt_hex(s32 rslt, void* argv){
    if(rslt==0){
        u32 flags = *(u16*)argv;
        u8 tmp[4];
        tmp[0] = flags&0xff;    flags >>= 8;
        tmp[1] = flags&0xff;
        tmp[2] = 0;
        tmp[3] = 0;
        pen_rsp(OPERATION_SUCCESS, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_PCBA_TEST, tmp, 4);
    }
    else{    
        pen_rsp(OPERATION_FAIL, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_PCBA_TEST, NULL, 0);
    }
}

// completed callback for pen.gcal %g
static void gCal_cmplt_str(s32 rslt, void* argv){
APP_LOG_DEBUG("<%s rslt:%d> ", __func__, rslt);
    if(rslt==0){    print("+ok@pen.gcal(%d)\r\n", *(u16*)argv);    }
    else{    print("+err@pen.gcal(%d)\r\n", rslt);    }
APP_LOG_DEBUG("</%s> ", __func__);
}
static void gCal_cmplt_hex(s32 rslt, void* argv){
    if(rslt==0){
        u8 tmp[2];
        u16 g = *(u16*)argv;
        tmp[0] = g&0xff;    g >>= 8;
        tmp[1] = g&0xff;
        pen_rsp(OPERATION_SUCCESS, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_PRESSURE_CALI, tmp, 2);
    }
    else{    
        pen_rsp(OPERATION_FAIL, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_PRESSURE_CALI, NULL, 0);
    }
}

// completed callback for pen.sleep
static void sleep_cmplt_str(s32 rslt, void* argv){
APP_LOG_DEBUG("<%s rslt:%d> ", __func__, rslt);
    CB_cps4041CB_mac_removed(NULL);
    if(rslt==0){    print("+ok@pen.sleep()\r\n");    }
    else{    print("+err@pen.sleep()\r\n");    }
APP_LOG_DEBUG("</%s> ", __func__);
}

static void sleep_cmplt_hex(s32 rslt, void* argv){
    CB_cps4041CB_mac_removed(NULL);
    if(rslt==0){    
        pen_rsp(OPERATION_SUCCESS, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_PENCIL_SLEEP, NULL, 0);
    }
    else{    
        pen_rsp(OPERATION_FAIL, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_PENCIL_SLEEP, NULL, 0);
    }
}

static void shipmode_cmplt_hex(s32 rslt, void* argv){
    if(rslt==0){    
        pen_rsp(OPERATION_SUCCESS, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_SHIPMODE, NULL, 0);
    }
    else{    
        pen_rsp(OPERATION_FAIL, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_SHIPMODE, NULL, 0);
    }
}

// completed callback for read_fw
static void read_fw_cmplt_hex(s32 rslt, void* argv){
    if(rslt==0){
        pen_rsp(OPERATION_SUCCESS, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_READ_FW, (uint8_t*)argv, 3);
    }
    else{    
        pen_rsp(OPERATION_FAIL, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_READ_FW, NULL, 0);
    }
}

// completed callback for pen.vib %op
static void vib_cmplt_str(s32 rslt, void* argv){
APP_LOG_DEBUG("<%s rslt:%d> ", __func__, rslt);
    if(rslt==0){    print("+ok@pen.vib(%d)\r\n", *(u8*)argv);    }
    else{    print("+err@pen.vib(%d)\r\n", rslt);    }
APP_LOG_DEBUG("</%s> ", __func__);
}
static void vib_cmplt_hex(s32 rslt, void* argv){
    if(rslt==0){    
        pen_rsp(OPERATION_SUCCESS, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_VIBRATE, NULL, 0);
    }
    else{    
        pen_rsp(OPERATION_FAIL, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_VIBRATE, NULL, 0);
    }
}

// completed callback for pen.rssi %op
static void rssi_cmplt_str(s32 rslt, void* argv){
APP_LOG_DEBUG("<%s rslt:%d> ", __func__, rslt);
    if(rslt==0){    print("+ok@pen.rssi(%d)\r\n", *(s8*)argv);    }
    else{    print("+err@pen.rssi(%d)\r\n", rslt);    }
APP_LOG_DEBUG("</%s> ", __func__);
}
static void rssi_cmplt_hex(s32 rslt, void* argv){
    if(rslt==0){    
        pen_rsp(OPERATION_SUCCESS, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_READ_RSSI, (u8*)argv, 1);
    }
    else{    
        pen_rsp(OPERATION_FAIL, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_READ_RSSI, NULL, 0);
    }
}

// completed callback for pen.rssi %op
static void disconnect_cmplt_str(s32 rslt, void* argv){
APP_LOG_DEBUG("<%s rslt:%d> ", __func__, rslt);
    if(rslt==0){    print("+ok@pen.disconnect('%02x:%02x:%02x:%02x:%02x:%02x')\r\n", oldMAC[5],oldMAC[4],oldMAC[3],oldMAC[2],oldMAC[1],oldMAC[0]);    }
    else{    print("+err@pen.disconnect()\r\n", rslt);    }
APP_LOG_DEBUG("</%s> ", __func__);
}
static void disconnect_cmplt_hex(s32 rslt, void* argv){
    if(rslt==0){    
        pen_rsp(OPERATION_SUCCESS, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_DISCONNECT, oldMAC, 6);
    }
    else{    
        pen_rsp(OPERATION_FAIL, PRODUCTION_TEST_ACK_HEAD, PRODUCTION_CLI_DISCONNECT, NULL, 0);
    }
}

int hexCmdCheck(const uint8_t *pData, uint16_t size, uint8_t* chckCorrect, uint16_t* wCmd, uint16_t* wLen){
    uint8_t checkCode;
    production_test_cli_t *pd_test_cli= (production_test_cli_t *)pData;
        
    *wLen = 0xff & (pd_test_cli->wLen>>8);
    *wCmd = 0xff & (pd_test_cli->wCmd>>8);

    *wLen = pData[1];    *wLen <<= 8;
    *wLen |= pData[2];
    *wCmd = pData[3];    *wCmd <<= 8;
    *wCmd |= pData[4];

    if(pd_test_cli->bHead != PRODUCTION_TEST_CLI_HEAD){
//        print("incorrect head: ");
//        for(int i=0;i<size;i++){    print("%02x ", pData[i]);    }
//        print("\r\n");
        return -1;
    }

    checkCode = test226_xorcheck(pData, size-1);
    *chckCorrect = checkCode;
        
    if(chckCorrect){    *chckCorrect = checkCode;    }
    
    if(pData[size-1] != checkCode){
//        print("check fail: ");
//        for(int i=0;i<size;i++){    print("%02x ", pData[i]);    }
//        print("\r\n");
        return -2;
    }

    return 0;
}

/**
 *****************************************************************************************
 * @brief 
 *
 * @param[in]
 *
 * @return 
 *****************************************************************************************
 */
static uint8_t test226_xorcheck(const uint8_t *buf, uint8_t len){
    uint8_t i = 0; 
    uint8_t checkxor = 0; 

    for (i = 0; i < len; i++) 
    { 
        checkxor = checkxor^buf[i]; 
    } 
    return ~checkxor; 
}

/**
 *****************************************************************************************
 * @brief 
 *
 * @param[in]
 *
 * @return 
 *****************************************************************************************
 */
void pen_rsp(uint8_t status,uint8_t head, uint8_t cmd, uint8_t* dat, uint8_t len){
    uint8_t databuff[128]={0};
    
    databuff[0]=head;
    databuff[1]=0;
    databuff[2]=7;
    databuff[3]=0;
    databuff[4]=cmd;
    databuff[5]=status;

    if((status==OPERATION_SUCCESS) && (dat!=NULL) && (len>0))
    {
        databuff[2]+=len;
        memcpy(&databuff[6], dat, len);
    }
        
    databuff[databuff[2]-1]=test226_xorcheck(databuff,databuff[2]-1);
    send_async(databuff, databuff[2]);
}

/**
 *****************************************************************************************
 * @brief 
 *
 * @param[in]
 *
 * @return 
 *****************************************************************************************
 */
u16 fetchHexCLFromRingBuffer(RINGBUFF_T* rb, u8* line, u16 len){
    u8 checkcode;
    u16 wCmd,wLen,ret=0;
    s32 i,bytes,count;
    u8 buff[256];
        
    count = RingBuffer_GetCount(rb);
    if((count <= 0) || (line==NULL) || (len==0))    return 0;
    
    // only take the lase receive
    while(count > 256){
        RingBuffer_Pop(rb, buff);    // abandoned
        count = RingBuffer_GetCount(rb);
    }
    memset(buff,0,256);
    bytes = RingBuffer_PopMult(rb, buff, 256);
    RingBuffer_Flush(rb);
    
    // seek for a packet
    for(i=0;i<bytes;i++){
        s32 x = hexCmdCheck(&buff[i], 256-i, &checkcode, &wCmd, &wLen);
        if(x==0){
            count = bytes-(i+wLen);
            if(count > 0){
                RingBuffer_InsertMult(rb, &buff[i+wLen], count);
            }
            ret = wLen;
            break;
        }
    }
    
    if(ret==0){    RingBuffer_InsertMult(rb, buff, bytes);        }    // restore

    return ret;
}

static void srv1char1_res(s32 rslt, void* argv){
    buff_t* x = (buff_t*)argv;
    char str[128] = {0}, *pStr;
    u8 i;
    
    for(i = 0; i < x->len; i++){
        pStr = &str[strlen(str)];
        sprintf(pStr,"%02x", x->buff[i]);
    }

    if(rslt==0){    print("+ok@srv1char1.req(%s)\r\n", str);    }
    else{    print("+err@srv1char1.req(%d)\r\n", rslt);    }
}

