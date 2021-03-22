#include <WiFi.h>
#include <stdarg.h>
#include "rl_debug.h"

uint16_t debugUdpPort;
IPAddress debugUdpIp;
WiFiUDP *udp;
int debugDestination;
extern char* rtc;


void setDebugOutput(int dest){
    debugDestination = dest;
}

void setUdpCfg(IPAddress ip, uint16_t port){
    debugUdpPort = port;
    debugUdpIp = ip;
    udp = new WiFiUDP();
}

void debugSend(const char* module, int level, const char* msg, ...){
    va_list args;
    va_start( args, msg );
    char debugMsg[100];
    char debugPayload[160];
    int length;
    vsprintf(debugMsg,msg,args);
    if (rtc)
        length = sprintf(debugPayload,"\r\n [%s] (%s) %s",rtc,module,debugMsg);
    else
        length = sprintf(debugPayload,"\r\n [%d] (%s) %s",millis(),module,debugMsg);

    if (debugDestination & DEBUG_SERIAL){
        Serial.print(debugPayload);
    }
    if (debugDestination & DEBUG_UDP){
        if (WiFi.status()== WL_CONNECTED){
            udp->beginPacket(debugUdpIp, debugUdpPort);
            udp->write((const uint8_t*)debugPayload, length);
            udp->endPacket();
        }
    }
    if (debugDestination & DEBUG_CLI){
            Serial.write(debugPayload);
    }


}