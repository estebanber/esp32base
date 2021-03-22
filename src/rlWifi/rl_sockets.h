
#include <WiFi.h>
typedef enum {
    RL_UDP_IDLE,
    RL_UDP_SEND,
    RL_UDP_WAIT_RESPONSE,
} udpState_t;

class UdpSender {
    WiFiUDP *udp;
    uint16_t udpLocalPort; 
    uint16_t udpRemotePort; 
    IPAddress udpRemoteIp; 
    void (*udpCallback)(uint8_t*,uint16_t);
    bool udpRemainsOpen;
    udpState_t udpNextState;
    udpState_t udpCurrentState;
    uint16_t udpBufferLength;
    uint8_t udpBuffer[512];
    bool udpResponse;
    uint32_t udpTimeout;
    bool udpIsListening=false;
public:
    UdpSender(
        uint16_t localport, 
        uint16_t remoteport, 
        IPAddress remoteip, 
        void (*callback)(uint8_t*,uint16_t),
        bool remainsopen
    );

    void udpMachine();
    void udpSendDatagram(uint8_t * data,uint16_t length);
    void udpSendDatagram(const char * data);
    bool udpIsResponse();
    uint16_t udpGetResponseLength();
    uint16_t udpGetResponse(uint8_t * buff);

};
typedef enum {
    RL_TCP_IDLE,
    RL_TCP_SEND,
    RL_TCP_WAIT_RESPONSE,
} tcpState_t;

class TcpSender {
    WiFiClient *tcp;
    uint16_t tcpLocalPort; 
    uint16_t tcpRemotePort; 
    IPAddress tcpRemoteIp; 
    void (*tcpCallback)(uint8_t*,uint16_t);
    bool tcpRemainsOpen;
    tcpState_t tcpNextState;
    tcpState_t tcpCurrentState;
    uint16_t tcpBufferLength;
    uint8_t tcpBuffer[512];
    bool tcpResponse;
    uint32_t tcpTimeout;
public:
    TcpSender(
        uint16_t localport, 
        uint16_t remoteport, 
        IPAddress remoteip, 
        void (*callback)(uint8_t*,uint16_t),
        bool remainsopen
    );

    void tcpMachine();
    void tcpSendDatagram(uint8_t * data,uint16_t length);
    void tcpSendDatagram(char * data);
    bool tcpIsResponse();
    uint16_t tcpGetResponseLength();
    uint16_t tcpGetResponse(uint8_t * buff);

};