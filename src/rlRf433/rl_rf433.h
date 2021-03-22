#include <Arduino.h>
#ifndef __RL_RF433_H__
#define __RL_RF433_H__

// MACROS AND LABELS ***********************************************************

#define	LED_ON					0
#define LED_OFF					1

#define GPIO_RF433_VT                           23                  

#define RF433_DELAY                             100   //1 mS
#define RF433_PANIC_TIMEOUT                     2000  //2 S

#define RF433_MAX_BUFFER                        41
#define RF433_FRAME_BUFFER                      5
#define RF433_DATA_SIZE                         3
#define RF433_FRAME_START                       0xFD
#define RF433_FRAME_END                         0xDF

#define RF433_STATUS_STARTED                    0x0001
#define RF433_STATUS_READED                     0x0002

#define RF433_COMMAND_START                     0x0001
#define RF433_COMMAND_FLUSH                     0x0002

typedef enum{
    RF433_STATE_INIT = 0,
    RF433_STATE_WAIT_CARD,
    RF433_STATE_WAIT_ELAPSEDTIME,
    RF433_STATE_REENTRY_PROTECTION,
}rf433States_t;

// OBJECTS *********************************************************************

typedef struct{

    rf433States_t oldState;
    rf433States_t state;

    unsigned long thousandth;
    unsigned long oldThousandth;
    uint32_t deltaThousandth;
    uint32_t timeout;
   
    uint32_t status;
    uint32_t cmd;
 
    bool isDebugEnabled;
    bool isEnabled;

//    String rfID;
    uint32_t rfIDCode;
	uint32_t rfIDCodeTemp;
	uint32_t elapsedTime;		// tiempo que se detecta presencia código
    
    char serialChar;
    char buffer[RF433_MAX_BUFFER];
    uint16_t index;
    bool startByte;

    bool isDeviceReaded;
    bool isRfIDCodeDetected;
	bool isRfIDCodeAvailable;

}rf433_t;

// EXTERNAL OBJECTS ************************************************************
// EXTERNAL FUNCTIONS **********************************************************

bool dr_rf433_getIsRfIDCodeDetectedFlag(void);
void dr_rf433_resetIsRfIDCodeDetectedFlag(void);

bool dr_rf433_getIsRfIDCodeAvailableFlag(void);
void dr_rf433_resetIsRfIDCodeAvailableFlag(void);

uint32_t dr_rf433_getRfIDCode(void);
uint32_t dr_rf433_getElapsedTime(void);

void dr_rf433_machine();
void dr_rf433_command(uint32_t cmd);
uint32_t dr_rf433_getStatus();

void dr_rf433_setDebug(bool toggle);
bool dr_rf433_getDebug();

void dr_rf433_setEnabled(bool toggle);
bool dr_rf433_getEnabled();

uint32_t rf433_readSerial();
void dr_rf433_start();

#endif // __RL_RF433_H__
