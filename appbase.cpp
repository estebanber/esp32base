#undef DHCPS_DEBUG
#include <Arduino.h>
#include "appbase.h"
#include "src/rlWifi/rlWifi.h"
#include "src/rlWifi/rl_sockets.h"
#include "src/rlDebug/rl_debug.h"
#include "src/rlRf433/rl_rf433.h"
#include "src/rlCLI/rlCLI.h"

#include <SPIFFS.h>

#undef DHCPS_DEBUG

void app_machine();
void getHashTable();
char *rtc = 0;
//IPAddress pcIp(192,168,0,103);
IPAddress pcIp(192,168,239,176);
UdpSender mainUdpSock = UdpSender(9999,9999,pcIp,nullptr,true);
uint32_t appTimeout=0; 





void setup() {
    pinMode(GPIO_STATUS_LED, OUTPUT);
    delay(100);
    int i;
    bool ledtest=false;
    /*
    for (i = 0; i < 10; i++) {
        delay(500);
        digitalWrite(GPIO_STATUS_LED, ledtest);
        ledtest=!ledtest;
    } 
    */
    
    
    
    Serial2.begin(9600);	// rfid
	Serial.begin(115200);	// usb
    delay(1000);

    wifi_init();
    dr_rf433_start();
    cli_init();
    
    setDebugOutput(DEBUG_UDP);
    setUdpCfg(IPAddress(192,168,239,176), 10000);

    accessPointData("ESP-ESTEBAN","Berna0123456");

    //wifiNetworkData("Esti-Ro","suri102014");
    wifiNetworkData("Rlink_Wifi","pSLHq*vaZP@@");
    
    debugSend("MAIN", 0,"Init");
    
    Serial.printf("\r\n\r\nBP WiFi Firmware - (c) R-Link S.A 2021\r\n");
    Serial.printf("\r\nVersion:     \t%s", FIRMWARE_VERSION);
    Serial.printf("\r\nDate:        \t%s %s", __DATE__, __TIME__);
    Serial.printf("\r\nWiFi:        \t%s,%s", "", "");  
    Serial.printf("\r\nWiFi AP:     \t%s,%s", "", "");
    Serial.printf("\r\nDevice:      \t%s", "");
    Serial.printf("\r\nNTP:         \t%s", "");
    Serial.printf("\r\nCFG:         \t%s,%s", "", "");

    Serial.print(pcIp);

    getHashTable();
    

}

void loop() {

    app_machine();
    wifi_machine();
    mainUdpSock.udpMachine();
    dr_rf433_machine();
    my_cli();
    //dr_web_machine();
    //dr_buzzer_machine();
    //dr_sled_machine();
    //dr_pled_machine();
}

void app_machine(){
    if (appTimeout< millis()){
        mainUdpSock.udpSendDatagram("Esteban Berna\r\n");
        debugSend("MAIN",0, "Sending data");
        appTimeout=millis()+5000;
    }
    if (mainUdpSock.udpIsResponse()){
        char buff[50],rcv[50];
        int largo = mainUdpSock.udpGetResponse((uint8_t *)buff);
        snprintf(rcv,largo+1,"%s",buff);
        debugSend("MAIN",0, "Recieving data: (%d)\"%s\"",largo,rcv);

        
    }

}

int (*cmdPtrList[7])(char args[][64]);
int cmdHashList[7];

int help(char args[][64]){
    if(!SPIFFS.begin(true)){
        Serial.println("An Error has occurred while mounting SPIFFS");
        return 0;
    }
    
    File file = SPIFFS.open("/index.html");
    if(!file){
        Serial.println("Failed to open file for reading");
        return 0;
    }
    
    Serial.println("File Content:");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

int setwifi(char args[][64]){
    Serial.println("Changing wifi...");
    wifiNetworkData(args[1],args[2]);
    return 0;
}
int getwifi(char args[][64]){
    Serial.println("=== WiFi Info ===");
    Serial.printf("SSID-Pass: %s, %s\r\n",wifiGetInfo(0),wifiGetInfo(1));
    Serial.printf("Local IP: %s\r\n",WiFi.localIP().toString().c_str());
    Serial.printf("AP-Pass: %s, %s\r\n",wifiGetInfo(2),wifiGetInfo(3));
    Serial.printf("Local IP: %s\r\n",WiFi.softAPIP().toString().c_str());
    Serial.println("=================");
    return 0;    
}
int rswifi(char args[][64]){
    wifi_init();
    return 0;
}
int setserdebug(char args[][64]){
    setDebugOutput(DEBUG_SERIAL);
}
int setudpdebug(char args[][64]){
    setDebugOutput(DEBUG_UDP);
}
int viewdebug(char args[][64]){
    suspendCli();
    setDebugOutput(DEBUG_CLI);
}
void getHashTable(){
    cmdHashList[0] = genHash("help");
    cmdHashList[1] = genHash("setwifi");
    cmdHashList[2] = genHash("rswifi");
    cmdHashList[3] = genHash("getwifi");
    cmdHashList[4] = genHash("setserdebug");
    cmdHashList[5] = genHash("setudpdebug");
    cmdHashList[6] = genHash("viewdebug");

    cmdPtrList[0] = &help;
    cmdPtrList[1] = &setwifi;
    cmdPtrList[2] = &rswifi;
    cmdPtrList[3] = &getwifi;
    cmdPtrList[4] = &setserdebug;
    cmdPtrList[5] = &setudpdebug;
    cmdPtrList[6] = &viewdebug;

    setCliCmd(cmdHashList,cmdPtrList,7);
}
