// Added Comment 2014-12-26
// NodeMsg.h
// Node message types

#ifndef NodeMsg_h
#define NodeMsg_h

#if (ARDUINO < 100)
#include "WProgram.h"
#else
#include <Arduino.h>
#endif

#define MAX_PKG_SIZE 32 // Package size in payload
#define WATCHDOG_DEFAULT 10000 // 10 seconds used by node
/*---------------------------------------------
|	!! PKG Types !!
|	Structure written to pkg[] array...
|
*/

typedef enum {
	// Endpoint ping
	pingMsg			= 0,	// System Ping
	
	// Sprinkler Zone Control types
	spklZone 	= 10, 	// Zone Control
	spklProg 	= 11, 	// Program Control	
	spklSys 	= 12, 	// Sprinkler System Control
	
	// Test Message
	testMsg			= 255	// Test messagte 
} NodeMsgType;

typedef enum {
	zone1 = 15, 	// Relay 0
	zone2 = 16, 	// Relay 1
	zone3 = 18, 	// Relay 2
	zone4 = 3, 		// Relay 3
	zone5 = 5, 		// Relay 4
	zone6 = 14, 	// Relay 5
	zone7 = 17, 	// Relay 6
	zone8 = 19, 	// Relay 7
	zone9 = 4, 		// Relay 8
	zone10 = 6 	  	// Relay 9
} SprinkleZoneIoMap;
// List of all zones
int zoneList[] = {
	zone1, zone2, zone3, zone4, zone5, 
	zone6, zone7, zone8, zone9, zone10
};
// Message Wrapper
typedef enum {
	GET, 
	SET, 
	STATUS, 
	READ, 
	WRITE,
	POST, 
	DELETE, 
	RESPONSE, 
	REQUEST
} msgMethodList;

typedef enum {
	// time in minutes
	// Number of Seconds / WATCHDOG_DEFAULT
	min1 	=	6, // 1 Minute for test 
	min5 	= 	30,
	min7	= 	42,
	min10 	= 	60,
} _zoneCycleTime;

// Wrapper structure 
typedef struct {		
  byte           	pkgType;
  byte 				msgMethod; // See Method List
  unsigned long  	msgID; 
  byte           	pkg[MAX_PKG_SIZE];  // 32 bytes
} Payload;
Payload theData;
/*---------------------------------------------
|
|	!! PKG Definitions !!
|	Sensor specific message structures
|
*/
typedef struct {
  byte      a;
  byte      b;
  byte      c;
  byte      d;
} _testMsg;
_testMsg testMsgPkg;

typedef struct {
	//Pkg for spklZoneCtrl 	= 10, 	// Zone Control
	byte 	zoneNumber;				// {1}
	byte	dataDirection; 			// {1} Send or receive 
	byte	zoneState;				// {1} {0=OFF, 1=ENABLE, 2=ON, 3=COMPLETE}
	byte 	zoneCycleTime;			// {2} total number of seconds to sprinkle
	byte	activeCycleTime; 		// {2} Actuve number of seconds for node
	byte	percentComplete;		// {1} Active time
	int 	pauseTime;				// {2} Ammount of time paused... (If in Pause Mode, otherwise 0)
} _spklZone;
_spklZone spklZonePkg;

typedef struct {
	// Sprinkler program control 
} _spklProg;
_spklProg spklProgPkg;

typedef struct {
  unsigned long lifetimeSprinkleSeconds;
  	 
  
} _spklSys;
_spklSys spklSysPkg;

typedef struct {
	// Configure sprinkle system defaults
  	
  	 
  
} _spklSysCfg;
_spklSysCfg spklSysCfgPkg;


/*---------------------------------------------------------------------------
|	REFERENCE - Datatype Size
|	
*/



#endif