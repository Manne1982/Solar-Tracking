#ifndef Project_Structures
#define Project_Structures
#include <Arduino.h>
#include <NTPClient.h>


#define OutputTurnOnOff D6
#define OutputTurnDirection D5
#define OutputRelais3 D4
#define OutputRelais4 D3
#define CounterPinPosition D7
#define LEDPin 1




enum {NW_WiFi_AP = 0x01, NW_StaticIP = 0x02, NW_EthernetActive = 0x04, NW_MQTTActive = 0x08, NW_MQTTSecure = 0x10}; //Enum for NWConfig
struct NWConfig {
  //Einstellungen NW-Einstellungen WLAN
  uint16 NW_Flags = 1; //See Enum NW_...
  char WLAN_SSID[40] = "SolarTracker";
  char WLAN_Password[70] = "";
  //Einstellungen NW-Einstellungen MQTT
  char MQTT_Server[50] = "192.168.178.2";
  uint16_t MQTT_Port = 1883;
  char MQTT_Username[20] = "Benutzer";
  char MQTT_Password[70] = "123456";
  char MQTT_fprint[70] = "";
  char MQTT_rootpath[100] = "/SolarTracker";
  //Einstellungen NW-Einstellungen Netzwerk
  char NW_StatischeIP = 0;
  char NW_NetzName[20] = "SolarTracker";
  char NW_IPAddress[17] = "192.168.4.1";
  char NW_SubMask[17] = "255.255.255.0";
  char NW_Gateway[17] = "192.168.4.2";
  char NW_DNS[17] = "192.168.4.2";
  char NW_NTPServer[55] = "fritz.box";
  char NW_NTPOffset = 0;
};

enum {tmHour = 0, tmMinute = 1};
enum {flagInitialised = 0x01, flagAutoModeOn = 0x02, flagError = 0x04};
enum {solOff, solWest, solEast, solWestRunAfter, solEastRunAfter};

struct ProjectConfig {
  uint16 Flags = 0; 
  uint32 MaxPosition;
  uint32 CurrentPosition;
  uint32 StartPosition;
  uint32 EndPosition;
  uint8 TimeStart[2];
  uint8 TimeEnd[2];
  uint8 TimeTurnBack[2];  
  uint16 BreakMinute;
};
class ProjectClass {
  public:
    ProjectClass();
    ~ProjectClass();
    void InitIO();
    void InitTimeClient(String _Server, long int _Offset);
    void timeClientUpdate();
    void checkSchedule();
    void goToAutoPosition();
    String getTimeString();
    void TurnSolar(uint8 _value = solOff); 
    void loop(const unsigned long * intCounter, unsigned long * intCounterOld); //Funktion have to included into main loop
    void goToPosition(uint32 _Value);
    void goToStart();
    void goToEnd();
    void goToTime(uint8 Direction, uint16 Time_ms);
    void startAutoMode();
    void stopAutoMode();
    uint8 getAutoStateFlag();
    uint8 getIsNotInit();

    ProjectConfig * getSettings();
    void setTimeStart(String _Clock);
    void setTimeStop(String _Clock);
    void setTimeTurnBack(String _Clock);
    void setTime(String _Start, String _End, String _TurnBack);
    void setTimeAutoBreak(uint16 _Minutes);
    uint16 getTimeAutoBreak();
    String getTimeStart();
    String getTimeEnd();
    String getTimeTurnBack();
    uint16 getMinutes(String _Time);
    void incrementCounter();
    void addToCounter(uint16 _value);
    void decrementCounter();
    void subToCounter(uint16 _value);
    void incrementCounterFailure();
    void addToCounterFailure(uint16 _value);
    uint16 resetCounterFailure(); //Return last Failure count befor reset
    uint16 getCouterFailure();
  
    void setMaxPosition(uint32 _Value);
    uint32 getMaxPosition();
   // void setCurrentPosition(uint32 _Value);
    uint32 getCurrentPosition();
    void setStartPosition(uint32 _Value);
    uint32 getStartPosition();
    void setEndPosition(uint32 _Value);
    uint32 getEndPosition();
    uint8 getOutputSolarState();
    bool anyChange();
    void StartReference();
    void AbortReference();
    uint8 getReferenceState();
    void resetErrorFlag();
    uint8 isError(); //is Error-Flag set
    void ChangeLED();
    const char * getFailurTimeStr();

  private:
    uint16 getMinutes(uint8 * _Time);
    bool checkTime(uint8 * _Time); //Return true if it is OK
    void ReferenceLoop();
    void ReferenceLoopLight();
    ProjectConfig * Settings;
    uint16 counterFailure;
    unsigned long OutputSolarLastChange;
    const uint16 OutputSolarChangeLock;
    uint8 OutputSolarState;  
    uint8 OutputSolarStateBefore;
    uint32 targetPosition;
    uint8 AutoPositioningOn;      //Automode ON
    uint8 anyPosChange;
    uint8 referenceState;
    uint8 referenceStateLight;
    unsigned long LastPosChange;
    unsigned long TimeToStop;
    unsigned long CounterPolling;
    uint8 HALLastState;
    uint32 LastPosChangeMinutes;
    uint8 LEDState;
    //Uhrzeit Variablen
    WiFiUDP *ntpUDP;
    NTPClient *timeClient;
    int monthDay;                                          //Variablen für die aktuelle Zeit
    int currentMonth;
    int currentYear;
    int currentHour;
    int currentMin;
    char FailureTime[50];
};

#include "Project_Structures.cpp"
#endif