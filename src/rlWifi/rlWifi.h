
/**
**************************** (C) R-Link S.A ************************************
* @file     dr_wifi.h
* @author   aberardi@rlink.com.ar
* @version  1.0
* @date     2020/09/26
**************************** (C) R-Link S.A ************************************
**/
#ifndef __RL_LIBWIFI_H__
#define __RL_LIBWIFI_H__

// MACROS AND LABELS ***********************************************************

#define PING_MAX_RETRIES                        1

#define WIFI_STATUS_STARTED                     0x0001
#define WIFI_STATUS_CONNECTED                   0x0002
#define WIFI_STATUS_STARTED_AP                  0x0004
#define WIFI_STATUS_NTP_READY                   0x0008
#define WIFI_STATUS_PING_READY                  0x0010
#define WIFI_STATUS_FILE_READY                  0x0020
#define WIFI_STATUS_TCP_READY                   0x0040
#define WIFI_STATUS_MQTT_READY                  0x0080
#define WIFI_STATUS_SENDED                      0x0100

#define WIFI_COMMAND_START                      0x0001
#define WIFI_COMMAND_CONNECT                    0x0002
#define WIFI_COMMAND_SYNC_NTP                   0x0008
#define WIFI_COMMAND_GET_FILE                   0x0010
#define WIFI_COMMAND_SEND_UDP                   0x0020
#define WIFI_COMMAND_SEND_TCP                   0x0040
#define WIFI_COMMAND_SEND_MQTT                  0x0080
#define WIFI_COMMAND_CONNECT_TCP                0x0100
#define WIFI_COMMAND_CONNECT_MQTT               0x0200

typedef enum{
    WIFI_STATE_IDLE = 0,
    WIFI_STATE_INIT,
    WIFI_STATE_WAIT_INIT,
    WIFI_STATE_CONNECT,
    WIFI_STATE_WAIT_CONNECT,
    WIFI_STATE_SYNC_NTP,
    WIFI_STATE_WAIT_SYNC_NTP,
    WIFI_PING_CHECK,
    WIFI_STATE_GETFILE,
    WIFI_STATE_WAIT_GETFILE,
    WIFI_STATE_WAIT_GETFILE_II,
    WIFI_STATE_SEND_UDP,
    WIFI_STATE_WAIT_SEND_UDP,
    WIFI_STATE_CONNECT_TCP,
    WIFI_STATE_WAIT_CONNECT_TCP,
    WIFI_STATE_SEND_TCP,
    WIFI_STATE_WAIT_SEND_TCP,
    WIFI_STATE_CONNECT_MQTT,
    WIFI_STATE_WAIT_CONNECT_MQTT,
    WIFI_STATE_SEND_MQTT,
    WIFI_STATE_WAIT_SEND_MQTT,
}wifiStates_t;

// OBJECTS *********************************************************************

typedef struct wifiScanList {
  char name[32]= "";
  int32_t rssi;
  struct wifiScanList *next;
}wifiScanList_t;


typedef struct{

    char ip[64];
    uint16_t port;
    bool hookConnection;
    bool waitForAnswer;
    String rxBuffer;
    String txBuffer;
    String path;
}socket_t;

typedef struct{

    wifiStates_t currentState;
    wifiStates_t nextState;
    
    uint32_t status;
    uint32_t cmd;
    uint8_t retries;
    uint8_t retries2;
    uint32_t timeout;

    bool isFoundNetwork;
    //bool isAPActive;
    bool isDebugEnabled;

}wifi_t;


// EXTERNAL OBJECTS ************************************************************
// EXTERNAL FUNCTIONS **********************************************************

void wifi_init();
void accessPointData(const char *name,const char*pass );
void wifiNetworkData(const char *name,const char*pass );
const char * wifiGetInfo(int param);
void wifi_machine();
void dr_wifi_command(uint32_t cmd);
uint32_t dr_wifi_getStatus();



bool dr_wifi_getIsConnected();
bool dr_wifi_getIsTCPConnected();


void dr_wifi_scan();
void dr_wifi_scanListInsert(String name,int32_t rssi);
void dr_wifi_scanList();


#endif // __DR_WIFI_H__
