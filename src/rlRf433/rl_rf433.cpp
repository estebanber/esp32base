#include <Arduino.h>
#include "rl_rf433.h"
#include "../rlDebug/rl_debug.h"


#define DEBUG 1
#if DEBUG
#define PRINTF(...) Serial.printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

// PRIVATE PROTOTYPES **********************************************************
const char * rf433_translateState(rf433States_t st);
rf433_t mRF433;
uint32_t aux32;

// EXTERNAL FUNCTIONS **********************************************************

void dr_rf433_start()
{
	//digitalWrite(GPIO_RF433_SW, 1);
	mRF433.startByte = 0;
	mRF433.index = 0;
	mRF433.state = RF433_STATE_INIT;
}

void dr_rf433_machine()
{
	mRF433.thousandth = millis();

	if (mRF433.thousandth > mRF433.oldThousandth)
	{
		mRF433.deltaThousandth = (uint32_t)(mRF433.thousandth - mRF433.oldThousandth);
	}
	else if (mRF433.thousandth < mRF433.oldThousandth)
	{
		mRF433.deltaThousandth = (uint32_t)((0x80000000 + mRF433.thousandth) - mRF433.oldThousandth);
	}

	mRF433.oldThousandth = mRF433.thousandth;

	mRF433.timeout += mRF433.deltaThousandth;

	if (mRF433.state != mRF433.oldState)
	{
        debugSend("RF433",0, "STATE %s", rf433_translateState(mRF433.state));
		mRF433.timeout = 0;
		mRF433.oldState = mRF433.state;
	}

	switch (mRF433.state) {

	case RF433_STATE_INIT:
	
		mRF433.index = 0;
		mRF433.state = RF433_STATE_WAIT_CARD;
		
		break;

	case RF433_STATE_WAIT_CARD:
	
		aux32 = rf433_readSerial();

		if (aux32 != 0)
		{
            debugSend("RF433",0, "Sensor: %03x", aux32);
			
			mRF433.rfIDCode = aux32;
			mRF433.isRfIDCodeDetected = true;
			mRF433.elapsedTime = millis();

			mRF433.state = RF433_STATE_WAIT_ELAPSEDTIME;
		}
		
		break;

	case RF433_STATE_WAIT_ELAPSEDTIME:
	
		// sale por cambio de estado en modulo RF
		if (digitalRead(GPIO_RF433_VT))
		{
			if(!mRF433.isRfIDCodeAvailable){
			
				mRF433.isRfIDCodeAvailable = true;
				mRF433.elapsedTime = millis() - mRF433.elapsedTime;
                debugSend("RF433",0, "Time: %d (l:%d)", mRF433.elapsedTime, __LINE__);
			}

			while (Serial2.available() > 0)
				Serial2.read();

			mRF433.state = RF433_STATE_WAIT_CARD;
		}

		else if((millis() - mRF433.elapsedTime) > RF433_PANIC_TIMEOUT){
			
            debugSend("RF433",0, "Time: %d (l:%d)", mRF433.elapsedTime, __LINE__);

			mRF433.isRfIDCodeAvailable = true;
			mRF433.elapsedTime = RF433_PANIC_TIMEOUT * 2;  // para que al comparar despues sea mayor

			mRF433.state = RF433_STATE_REENTRY_PROTECTION;
		}

		break;

	case RF433_STATE_REENTRY_PROTECTION:

			if (digitalRead(GPIO_RF433_VT)){

				mRF433.elapsedTime = 0;

				while (Serial2.available() > 0)
				Serial2.read();

				mRF433.state = RF433_STATE_WAIT_CARD;
			}

		break;
	}
}

uint32_t dr_rf433_getElapsedTime(void)
{
	return mRF433.elapsedTime;
}

uint32_t dr_rf433_getRfIDCode(void)
{
	return mRF433.rfIDCode;
}

bool dr_rf433_getIsRfIDCodeAvailableFlag(void)
{
	return mRF433.isRfIDCodeAvailable;
}

void dr_rf433_resetIsRfIDCodeAvailableFlag(void)
{
	mRF433.isRfIDCodeAvailable = false;
}

bool dr_rf433_getIsRfIDCodeDetectedFlag(void)
{
	return mRF433.isRfIDCodeDetected;
}

void dr_rf433_resetIsRfIDCodeDetectedFlag(void)
{
	mRF433.isRfIDCodeDetected = false;
}

void dr_rf433_command(uint32_t cmd)
{
	mRF433.cmd |= cmd;
}

uint32_t dr_rf433_getStatus()
{
	return mRF433.status;
}

void dr_rf433_setDebug(bool toggle)
{
	mRF433.isDebugEnabled = toggle;
}

bool dr_rf433_getDebug()
{
	return mRF433.isDebugEnabled;
}

void dr_rf433_setEnabled(bool toggle)
{
	mRF433.isEnabled = toggle;
}

bool dr_rf433_getEnabled()
{
	return mRF433.isEnabled;
}

// PRIVATE FUNCTIONS ***********************************************************

const char * rf433_translateState(rf433States_t st)
{

	switch (st){
	case RF433_STATE_INIT:
		return "INIT";
		break;
	case RF433_STATE_WAIT_CARD:
		return "WAITING RFID";
		break;
	case RF433_STATE_WAIT_ELAPSEDTIME:
		return "WAITING ELAPSEDTIME";
		break;
	case RF433_STATE_REENTRY_PROTECTION:
		return "REENTRY PROTECTION";
		break;
	default:
		return "UNKNOWN";
	}
}

uint32_t rf433_readSerial()
{
	if (Serial2.available() > 0)
	{
		char ch = Serial2.read();
		
		if (mRF433.startByte==0 && ch == RF433_FRAME_START){
			mRF433.startByte=1;
			mRF433.index = 0;
		}

		if (mRF433.startByte && ch != RF433_FRAME_END)
		{
			mRF433.buffer[mRF433.index++] = ch;
			return 0;
		}
		else
		{
			if (ch == RF433_FRAME_END){
				mRF433.index = 0;
				mRF433.startByte= 0;
				return (uint32_t)(mRF433.buffer[1] << 16 | mRF433.buffer[2] << 8 | mRF433.buffer[3]);
			}
		}
	}
	return 0;
}
