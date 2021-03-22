#include "rl_sockets.h"
#include "../rlDebug/rl_debug.h"




UdpSender::UdpSender(
    uint16_t localport, 
    uint16_t remoteport, 
    IPAddress remoteip, 
    void (*callback)(uint8_t*,uint16_t),
    bool remainsopen
    ){
        udpLocalPort=localport;
        udpRemotePort=remoteport;
        udpRemoteIp = remoteip;
        udpCallback = callback;
        udpRemainsOpen= remainsopen;
        udpCurrentState=RL_UDP_IDLE;
        udpNextState=RL_UDP_IDLE;
        udp = new WiFiUDP();
        

}

void UdpSender::udpMachine(){
    if (udpNextState != udpCurrentState){
        udpCurrentState = udpNextState;
        debugSend("UDP", 0, "UPD State %d" , udpNextState );
        //PRINTF("\r\n (UDP) STATE %d", udpCurrentState);
    }

    switch (udpCurrentState){
        case RL_UDP_SEND:
            debugSend("UDP",0, "UPD Sending data to %s",udpRemoteIp.toString().c_str());
            if (!udpRemainsOpen) udp->begin(udpLocalPort);
            udp->beginPacket(udpRemoteIp, udpRemotePort);
            udp->write(udpBuffer, udpBufferLength);
            udp->endPacket();
            udpResponse=false;
            udpNextState = RL_UDP_WAIT_RESPONSE;
            udpTimeout = millis()+1000;
            break;
        case RL_UDP_WAIT_RESPONSE:
            if (udpTimeout>millis()){
                udpBufferLength=udp->parsePacket();
                if (udpBufferLength){
                    debugSend("UDP",0, "UPD Receiving data: %d bytes",udpBufferLength);
                    udp->read(udpBuffer, udpBufferLength);
                    udpResponse=true;
                    if (udpCallback){
                        udpCallback(udpBuffer,udpBufferLength);
                        udpNextState = RL_UDP_IDLE;
                    }
                }
            }
            else{
                if (!udpRemainsOpen) udp->stop();
                udpNextState = RL_UDP_IDLE;
            }
            break;
        case RL_UDP_IDLE:
        default:
            if (udpRemainsOpen){
                if (WiFi.status()== WL_CONNECTED && !udpIsListening){
                    udpIsListening = udp->begin(udpLocalPort);
                }
                    
                udpNextState = RL_UDP_WAIT_RESPONSE;
                udpTimeout = millis()+10000;
            }
    }
}

void UdpSender::udpSendDatagram(uint8_t * data,uint16_t length){
    memcpy(udpBuffer,data,length);
    udpBufferLength=length;
    udpNextState= RL_UDP_SEND;
}

void UdpSender::udpSendDatagram(const char * data){
    udpSendDatagram((uint8_t*)data,strlen(data));
}

bool UdpSender::udpIsResponse(){
    return udpResponse;
}

uint16_t UdpSender::udpGetResponseLength(){
    return udpBufferLength;
}

uint16_t UdpSender::udpGetResponse(uint8_t * buff){
    if (udpResponse){
        memcpy(buff,udpBuffer,udpBufferLength);
        udpResponse=false;
        return udpBufferLength;
    }
    return 0;
}

void config_TCP(){

}
void config_MQTT(){

}


void machine_TCP(){

}

void machine_MQTT(){

}