
#ifndef __RL_DEBUG_H__
#define __RL_DEBUG_H__


#define DEBUG_SERIAL 0x0001
#define DEBUG_UDP 0x0002
#define DEBUG_FILE 0x0004
#define DEBUG_CLI 0x0008


void setDebugOutput(int dest);
void setUdpCfg(IPAddress ip, uint16_t port);
void debugSend(const char* module,int level, const char* msg, ...);

#endif //RL_DEBUG_H