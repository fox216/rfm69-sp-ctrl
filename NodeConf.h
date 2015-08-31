// !!!!  DEVICE CONFIGURATION SETTINGS  !!!!
#ifndef NodeConf_h
#define NodeConf_h

#if (ARDUINO < 100)
#include "WProgram.h"
#else
#include <Arduino.h>
#endif

/*---------------------------------------------
|
|	Node Configuration Settings
|	
*/

#define NODEID      42
#define NETWORKID   100
#define	GATEWAYID	99
#define TARGETID_2  1
#define FREQUENCY   RF69_915MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
#define KEY         "thisIsEncryptKey" //has to be same 16 characters/bytes on all nodes, not more not less!
#define LED         9
#define SERIAL_BAUD 115200
#define ACK_TIME    30  // # of ms to wait for an ack
#define WAIT_TIME   1000
#define TRANSMITPERIOD 30000 
#define EVENTPERIOD 12000
#define EPROM_BUFFER 10



//byte epBuffer[] 
/*
typedef struct { // Total structure Size {}
	byte zoneNumber;
	unsigned long totalSprinkleTime;
} _epZoneData;
_epZoneData epZoneData;
*/


/*
used by system moving to global header
typedef struct {
	zoneA currZone;
	unsigned long zoneStartTime; // store Millis since start
} _sysZoneState;


typedef struct {
	//Pkg for spklZoneCtrl 	= 10, 	// Zone Control
	
	byte	zoneState;				// {1} {0=OFF, 1=ON, 2=PAUSED}
	byte 	zoneCycleTime;			// {2} total number of seconds to sprinkle
	byte	activeCycleTime; 	// {2} Actuve number of seconds for node
	byte	percentComplete;		// {1} Active time
	int 	pauseTime;				// {2} Ammount of time paused... (If in Pause Mode, otherwise 0)
} _sysMetrics;
_sysMetrics sysMetrics;

*/






#endif