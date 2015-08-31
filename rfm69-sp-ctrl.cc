#include <RFM69.h>
#include <SPI.h>
#include <NodeMsg.h>
#include <NodeConf.h>
#include <EEPROM.h>

// REFERENCES 
RFM69 radio;
// FUNCTIONS - Local
void Blink(byte PIN, int DELAY_MS);
void setZone();
void disableAllZones();
//void setZoneStatus();
void sendZoneStatus();
// GLOBAL Variables
unsigned int zoneSprinkleTimeSeconds = 0;
bool zoneActive = false;
//byte activeZone = 0; // current zone
byte ackCount=0;
bool timerActive = false;
int lastPeriod = -1;
bool promiscuousMode = false; //set to 'true' to sniff all packets on the same network
float b;
// SETUP... 

void setup() {
  /*---------------------------------------------------------------------------------
  |     System Setup
  |
  |
  */
  Serial.begin(SERIAL_BAUD);
  delay(10);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  //radio.setHighPower(); //uncomment only for RFM69HW!
  radio.encrypt(KEY);
  radio.promiscuous(promiscuousMode);
  // Print Radio Config...
  char buff[50];
  sprintf(buff, "\nListening at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.println(buff);
  // Configure all zone IO as Outputs
  for (int x = 0; x < sizeof(zoneList); x++) {
    // set IO Zones to digital outputs. 
    // Set inital state of OFF
    pinMode(zoneList[x], OUTPUT);
    digitalWrite(zoneList[x], LOW);
  }
}
// EXECUTION Loop
void loop() {
  /*---------------------------------------------------------------------------------
  |     Execution Loop
  |
  |
  */
  if (radio.receiveDone()) {
    Serial.print('[');Serial.print(radio.SENDERID, DEC);Serial.print("] ");
    Serial.print(" [RX_RSSI:");Serial.print(radio.readRSSI());Serial.print("]");
    if (promiscuousMode) {
      Serial.print("to [");Serial.print(radio.TARGETID, DEC);Serial.print("] ");
    }
    if (radio.DATALEN != sizeof(Payload))
      Serial.print("Invalid payload received, not matching Payload struct!");
    else
    {
      // Get the date
      theData = *(Payload*)radio.DATA; //assume radio.DATA actually contains our struct and not something else
      Serial.print(" S=");
      Serial.print(radio.SENDERID);
      Serial.print(" T=");
      Serial.println(radio.TARGETID);
      Serial.print(" msgID=");
      Serial.print(theData.msgID);
      Serial.print(" pkgType=");
      Serial.print(theData.pkgType);
      Serial.print(" Size=");
      Serial.println(radio.DATALEN);

      Serial.println("Package Info..... ");


      // Find method to process based on package type
      switch(theData.pkgType) {
        case spklZone:
          switch(theData.msgMethod) {
            case SET:
              Serial.println("--- Executing Sprinkle Zone SET Method \n");
              setZone();
              break;
            case STATUS:
              Serial.println("\n--- Executing Sprinkle Zone STATUS Method \n");
              // return the zone status to the requestor....
              sendZoneStatus();
              break;
            default:
              Serial.println(" Method is not supported ... "); 
          }
          break;  
        default:
          Serial.println(" No Function defined... ");

      }
    }
    
    if (radio.ACK_REQUESTED)
    {
      byte theNodeID = radio.SENDERID;
      radio.sendACK();
      Serial.print(" - ACK sent.");

      // When a node requests an ACK, respond to the ACK
      // and also send a packet requesting an ACK (every 3rd one only)
      // This way both TX/RX NODE functions are tested on 1 end at the GATEWAY
      if (ackCount++ %3 == 0)
      {
        Serial.print(" Pinging node ");
        Serial.print(theNodeID);
        Serial.print(" - ACK...");
        delay(3); //need this when sending right after reception .. ?
        if (radio.sendWithRetry(theNodeID, "ACK TEST", 8, 0))  // 0 = only 1 attempt, no retries
          Serial.print("ok!");
        else Serial.print("nothing");
      }
    } 
    Serial.println();
    Blink(LED,3);
  }

  /*---------------------------------------------------------------------------------
  |     Watchdog timer
  | 1. check run cycle every 10 seconds
  | 2. Verify sprinkle time is not over run time MAX
  */
  // Watchdog Timer
  // Execute an action every 10 seconds...
  if (millis() % WATCHDOG_DEFAULT == 0) { // Defined NodeMsg.h
    if (! timerActive)  {
      Serial.print("Timer Active, State: ");
      Serial.println(spklZonePkg.zoneState);
      if (spklZonePkg.zoneState == 1) {
        // Increment Sprinkle timer
        Serial.println("Execute WatchDog ");
      
        spklZonePkg.activeCycleTime ++;        
        if (spklZonePkg.activeCycleTime >= spklZonePkg.zoneCycleTime) {
          // Zone time complete.
          // Disable and reset zone  
          spklZonePkg.zoneState = 3;
          disableAllZones();
        }
        //float a = (float)spklZonePkg.activeCycleTime;
        b = (float)spklZonePkg.zoneCycleTime;
        
        
        spklZonePkg.percentComplete = 100 * (spklZonePkg.activeCycleTime /  b);
        Serial.print("% Complete: ");
        Serial.println(spklZonePkg.percentComplete);
          
      }
      timerActive = true;
    }
  } else {
    // handle debounce
    timerActive = false;
  }
  
  /*
  int currPeriod = millis()/EVENTPERIOD;
  if (currPeriod != lastPeriod) {
    // Real time timer 
    disableAllZones();
    lastPeriod = currPeriod;
  }
  */
  

}
/// END EXECUTION LOOP 

void Blink(byte PIN, int DELAY_MS) {
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}

//void setZone(spklZonePkg& pkg) {
void setZone() {
  // Process recieved Zone instruction
  // CAST to package type
  spklZonePkg = *(_spklZone*)theData.pkg; // Cast package as zone structure
  Serial.print(" zoneNumber =");
  Serial.println(spklZonePkg.zoneNumber);
  Serial.print(" dataDirection =");
  Serial.println(spklZonePkg.dataDirection);
  Serial.print(" zoneState =");
  Serial.println(spklZonePkg.zoneState);
  Serial.print(" totalSprinkleTime =");
  Serial.println(spklZonePkg.zoneCycleTime);
  Serial.print(" activeSprinkleTime =");
  Serial.println(spklZonePkg.activeCycleTime);
  Serial.print(" percentComplete =");
  Serial.println(spklZonePkg.percentComplete);
  Serial.print(" pauseTime =");
  Serial.println(spklZonePkg.pauseTime);
  /*
  if (spklZonePkg.zoneNumber == 3) {
    // Flip outout 
    digitalWrite(6, ! digitalRead(6));
  }
  */
  // Enable Requested Zone.
  //activeZone = spklZonePkg.zoneNumber;
  //zoneActive = true;
  disableAllZones();
  digitalWrite(zoneList[spklZonePkg.zoneNumber], HIGH);
  
}
void sendMsg() {

}

void sendZoneStatus() {
  /*---------------------------------------------------------------------------------
  |   sendZoneStatus -  
  |   Send last note status state to the calling application.
  |
  |
  */
  radio.sendACK();
  theData.msgID = millis(); // Suggest changing method to return original msgID
  theData.pkgType = spklZone;
  theData.msgMethod = STATUS;
  memcpy(theData.pkg, &spklZonePkg, sizeof(spklZonePkg));
  if (radio.sendWithRetry(GATEWAYID, (const void*)(&theData), sizeof(theData)))  // 0 = only 1 attempt, no retries
    Serial.print("ok!");
  else Serial.print("nothing");
}


void disableAllZones(){
  // Turn all zones off
  for (int x = 0; x < sizeof(zoneList); x++) {
    digitalWrite(zoneList[x], LOW);
  }
}
