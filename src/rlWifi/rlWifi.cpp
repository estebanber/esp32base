
/**
**************************** (C) R-Link S.A ************************************
* @file     dr_wifi.ino
* @author   aberardi@rlink.com.ar
* @version  1.0
* @date     2020/09/26
**************************** (C) R-Link S.A ************************************
**/
#include <Arduino.h>
#include <WiFi.h>
#include <HardwareSerial.h>
#include <WString.h>
#include <ESP32Ping.h>
#include "rlWifi.h"
#include "../rlDebug/rl_debug.h"

#define SECONDS(t) t*1000
#define WIFI_TIMEOUT_NTP_CHECK 60000
#define DEFAULT_PING_SERVER "8.8.8.8"
#define NTP_MAX_RETRIES 3
// PRIVATE PROTOTYPES **********************************************************

String wifi_translateState(wifiStates_t state);
String wifi_translateConectionStatus(uint8_t status);
void dr_wifi_deleteScanList();

uint8_t wifiConectionStatus;
uint8_t server = 0;
wifiScanList_t *wifiRoot=nullptr;
wifiScanList_t *wifiTail=nullptr;
wifi_t mWifi;
char apssid[20]="";
char appass[20]="";
char nssid[20]="";
char npass[20]="";
// EXTERNAL FUNCTIONS **********************************************************

void wifi_init(){
    mWifi.currentState=WIFI_STATE_INIT;
    mWifi.nextState=WIFI_STATE_INIT;
}
void accessPointData(const char *name,const char*pass ){
    strcpy(apssid,name);
    strcpy(appass,pass);
}
void wifiNetworkData(const char *name,const char*pass ){
    strcpy(nssid,name);
    strcpy(npass,pass);
}
const char * wifiGetInfo(int param){
    switch (param) {
    case 0:
        return nssid;
    case 1:
        return npass;
    case 2:
        return apssid;
    case 3:
        return appass;
    
    default:
        return 0;
    }
}

void wifi_machine() {
    if (wifiConectionStatus != WiFi.status() ){
        wifiConectionStatus = WiFi.status();
        debugSend("WIFI",0, "Connection status %s [%02d]", wifi_translateConectionStatus(wifiConectionStatus).c_str(), wifiConectionStatus);
    }
    if (mWifi.nextState != mWifi.currentState){
        mWifi.currentState = mWifi.nextState;
        debugSend("WIFI",0,"Change to state: %s", wifi_translateState(mWifi.currentState).c_str());
    }

    switch (mWifi.currentState) {

    case WIFI_STATE_INIT:
    {
        debugSend("WIFI",0,"Complete initialization");
        Serial.println(1);
        WiFi.disconnect(true);
        Serial.println(2);
        //delay(1);
        Serial.println(3);
        WiFi.softAPdisconnect(true);
        Serial.println(4);
        mWifi.cmd=0x100; //Initial State
        mWifi.retries = 0;

        mWifi.isFoundNetwork = false;




        WiFi.mode(WIFI_AP_STA);
        debugSend("WIFI",0, "Activating AP %s (%s)", apssid, appass);
        WiFi.softAP(apssid, appass, 1, 0, 1);

        //delay(100);
        //dr_wifi_scan();
        mWifi.timeout = millis() + SECONDS(1);
        mWifi.nextState = WIFI_STATE_WAIT_INIT;

        break;
    }

    case  WIFI_STATE_WAIT_INIT:
        if (mWifi.timeout < millis()){
            mWifi.status |= WIFI_STATUS_STARTED;
            mWifi.nextState = WIFI_STATE_CONNECT;
        }
        break;
    case WIFI_STATE_CONNECT:
        WiFi.begin(nssid, npass);
        debugSend("WIFI",0, "Trying to connect to %s", nssid);
        mWifi.timeout = millis() + 3000;//WIFI_TIMEOUT_CONNECTING;
        mWifi.nextState = WIFI_STATE_WAIT_CONNECT;
        break;
    case WIFI_STATE_WAIT_CONNECT:
        if (WiFi.isConnected()) {
            WiFi.setAutoReconnect(true);
            mWifi.isFoundNetwork = true;
            debugSend("WIFI",0,"Connected to \"%s\" with IP: %s", nssid,WiFi.localIP().toString().c_str());
            mWifi.status |= WIFI_STATUS_CONNECTED;
            // mWifi.nextState = WIFI_STATE_IDLE;
            mWifi.nextState = WIFI_STATE_IDLE;
            mWifi.retries = 0;
        }

        if (mWifi.timeout < millis()) {
            // WIFI_ERROR
            debugSend("WIFI",0,"Not connected (l:%d)", __LINE__);
            //WiFi.disconnect(true);
            //delay(100);
            //dr_wifi_scan();
            mWifi.isFoundNetwork = false;
            // mWifi.nextState = WIFI_STATE_IDLE;
            mWifi.nextState = WIFI_STATE_CONNECT;
        }
        break;
    case WIFI_STATE_IDLE:
    default:
        if (mWifi.cmd & WIFI_COMMAND_START) {
            mWifi.status = 0;
            mWifi.cmd =0; //&= ~WIFI_COMMAND_START;
            mWifi.timeout = 0;
            mWifi.retries = 0;
            mWifi.nextState = WIFI_STATE_INIT;
        }

        break;
    }
}

void dr_wifi_command(uint32_t cmd)
{
    mWifi.cmd |= cmd;
}


void dr_wifi_setDebug()
{
    mWifi.isDebugEnabled = true;
}

void dr_wifi_resetDebug()
{
    mWifi.isDebugEnabled = false;
}

bool dr_wifi_getDebug()
{
    return mWifi.isDebugEnabled;
}

uint32_t dr_wifi_getStatus()
{
    return mWifi.status;
}

bool dr_wifi_getIsConnected()
{
    return WiFi.isConnected();
}

void dr_wifi_showActualState(void)
{
    Serial.printf("\n\n WIFI STATUS");
    Serial.printf("\r\n <Machine State>    %s", wifi_translateState(mWifi.currentState).c_str());
    Serial.printf("\r\n <Machine Status>   [NA]");
    Serial.printf("\r\n <Network SSID>     %s [%s]", "aaaa", "pppppp");
    Serial.printf("\r\n <Network status>   %s", WiFi.isConnected() ? "Connected" : "Disconnected");
    Serial.printf("\r\n <AP>               %s [%s]", "apapap", "pwpwpw");
    Serial.printf("\r\n <Server status>    ");

    //ver - revisar, explota cuando no está conectado a red wifi
    /*
    if(mWifi.mSocketTCP.tcp->available() > 0){
        Serial.printf("%s", mWifi.mSocketTCP.tcp->connected() ? "Connected" : "Disconnected");
    }
    else{
        Serial.printf("Not Available");
    }
    */
}

// PRIVATE FUNCTIONS ***********************************************************

String wifi_translateConectionStatus(uint8_t status){
    String strState = "";

    switch (status)
    {
    case WL_IDLE_STATUS:
        strState = "WL_IDLE_STATUS";
        break;
    case WL_NO_SSID_AVAIL:
        strState = "WL_NO_SSID_AVAIL";
        break;
    case WL_SCAN_COMPLETED:
        strState = "WL_SCAN_COMPLETED";
        break;
    case WL_CONNECTED:
        strState = "WL_CONNECTED";
        break;
    case WL_CONNECT_FAILED:
        strState = "WL_CONNECT_FAILED";
        break;
    case WL_CONNECTION_LOST:
        strState = "WL_CONNECTION_LOST";
        break;
    case WL_DISCONNECTED:
        strState = "WL_DISCONNECTED";
        break;
    }

    return strState;
}

String wifi_translateState(wifiStates_t state)
{

    String strState = "";

    switch (state)
    {
    case WIFI_STATE_IDLE:
        strState = "IDLE";
        break;
    case WIFI_STATE_INIT:
        strState = "INIT";
        break;
    case WIFI_STATE_CONNECT:
        strState = "CONNECT";
        break;
    case WIFI_STATE_WAIT_CONNECT:
        strState = "WAIT CONNECT";
        break;
    case WIFI_STATE_SYNC_NTP:
        strState = "SYNC NPT";
        break;
    case WIFI_STATE_WAIT_SYNC_NTP:
        strState = "WAIT NPT";
        break;
    case WIFI_STATE_GETFILE:
        strState = "GET FILE";
        break;
    case WIFI_STATE_WAIT_GETFILE:
        strState = "WAIT GET FILE";
        break;
    case WIFI_STATE_SEND_UDP:
        strState = "SEND UDP";
        break;
    case WIFI_STATE_WAIT_SEND_UDP:
        strState = "WAIT ANSWER UDP";
        break;
    case WIFI_STATE_CONNECT_MQTT:
        strState = "CONNECT MQTT";
        break;
    case WIFI_STATE_WAIT_CONNECT_MQTT:
        strState = "WAIT CONNECT MQTT";
        break;
    case WIFI_STATE_SEND_MQTT:
        strState = "SEND MQTT";
        break;
    case WIFI_STATE_WAIT_SEND_MQTT:
        strState = "WAIT ANSWER MQTT";
        break;
    case WIFI_STATE_CONNECT_TCP:
        strState = "CONNECT TCP";
        break;
    case WIFI_STATE_WAIT_CONNECT_TCP:
        strState = "WAIT CONNECT TCP";
        break;
    case WIFI_STATE_SEND_TCP:
        strState = "SEND TCP";
        break;
    case WIFI_STATE_WAIT_SEND_TCP:
        strState = "WAIT ANSWER TCP";
        break;
    default:
        strState = "UNKNOWN (" + String(state) + ")";
    }

    return strState;
}

String dr_wifi_translateIsConnected(uint8_t type)
{
  String strType = "";

  switch (type)
  {
    case 0:
      strType = "Desconectado";
      break;
    case 1:
      strType = "Conectado";
      break;
    default:
      strType = "UNKNOWN";
          break;
  }

  return strType;
}

// scan wifi
void dr_wifi_scan(){
    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    //PRINTF("Redes: %d",n);
    if (n > 0) {
        dr_wifi_deleteScanList();
        for (int i = 0; i < n; ++i) {
            if (WiFi.RSSI(i)<=-80)
                continue;
            dr_wifi_scanListInsert(String(WiFi.SSID(i)),WiFi.RSSI(i));
            delay(10);
        }
    }
}
void dr_wifi_deleteScanList(){
    wifiScanList_t *nodo,*delNodo;
    if (wifiRoot== NULL)
        return;
    nodo=wifiRoot;
    while(nodo){
        delNodo=nodo;
        nodo=nodo->next;
        free(delNodo);
    }
    wifiTail=NULL;
    wifiRoot=NULL;

}
void dr_wifi_scanListInsert(String name, int32_t rssi){
    wifiScanList_t *nodo;
    nodo = (wifiScanList_t *)malloc(sizeof(wifiScanList_t));
    if(nodo){
      nodo->rssi = rssi;
      name.toCharArray(nodo->name, name.length()+1);
      nodo->next = NULL;
      if (wifiRoot == NULL){
          wifiRoot = nodo;
          wifiTail = nodo;
      }
      else{
          wifiTail->next = nodo;
          wifiTail = nodo;
      }
    }
}
void dr_wifi_scanList(){
  wifiScanList_t *nodo= wifiRoot;
  Serial.println("\n(WIFI) SCAN RED:");
  while(nodo != NULL){
    Serial.printf("\n\r%s (%d dBm)",nodo->name,nodo->rssi);
    nodo= nodo->next;
  }
}
time_t cvt_TIME(char const *time) { 
    char s_month[5];
    int month, day, year;
    struct tm t = {0};
    static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

    sscanf(time, "%s %d %d", s_month, &day, &year);

    month = (strstr(month_names, s_month)-month_names)/3;

    t.tm_mon = month;
    t.tm_mday = day;
    t.tm_year = year - 1900;
    t.tm_isdst = -1;

    return mktime(&t);
}