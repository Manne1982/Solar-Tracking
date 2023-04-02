#ifndef Project_Structures
#define Project_Structures
#include <Arduino.h>

#define OutputTurnOnOff D6
#define OutputTurnDirection D5
#define OutputRelais3 D4
#define OutputRelais4 D3
#define CounterPinPosition D7




enum {NW_WiFi_AP = 0x01, NW_StaticIP = 0x02, NW_EthernetActive = 0x04, NW_MQTTActive = 0x08, NW_MQTTSecure = 0x10}; //Enum for NWConfig
struct NWConfig {
  //Einstellungen NW-Einstellungen WLAN
  uint16 NW_Flags = 1; //See Enum NW_...
  char WLAN_SSID[40] = "Regner";
  char WLAN_Password[70] = "";
  //Einstellungen NW-Einstellungen MQTT
  char MQTT_Server[50] = "192.168.178.2";
  uint16_t MQTT_Port = 1883;
  char MQTT_Username[20] = "Benutzer";
  char MQTT_Password[70] = "123456";
  char MQTT_fprint[70] = "";
  char MQTT_rootpath[100] = "/Solar";
  //Einstellungen NW-Einstellungen Netzwerk
  char NW_StatischeIP = 0;
  char NW_NetzName[20] = "Regner";
  char NW_IPAddress[17] = "192.168.178.10";
  char NW_SubMask[17] = "255.255.255.0";
  char NW_Gateway[17] = "192.168.178.1";
  char NW_DNS[17] = "192.168.178.1";
  char NW_NTPServer[55] = "fritz.box";
  char NW_NTPOffset = 0;
};

enum {tmHour = 0, tmMinute = 1};
enum {flagInitialised = 0x01, flagAutoModeOn = 0x02};
enum {solOff, solWest, solEast};

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
    void TurnSolar(uint8 _value = solOff); 
    void loop(); //Funktion have to included into main loop
    void goToPosition(uint32 _Value);
    void goToStart();
    void goToEnd();

    ProjectConfig * getSettings();
    void setTimeStart(String _Clock);
    void setTimeStop(String _Clock);
    void setTimeTurnBack(String _Clock);
    void setTime(String _Start, String _End, String _TurnBack);
    void incrementCounter();
    void decrementCounter();
    void incrementCounterFailure();
    uint16 resetCounterFailure(); //Return last Failure count befor reset
    uint16 getCouterFailure();

    void setMaxPosition(uint32 _Value);
    uint32 getMaxPosition();
    void setCurrentPosition(uint32 _Value);
    uint32 getCurrentPosition();
    void setStartPosition(uint32 _Value);
    uint32 getStartPosition();
    void setEndPosition(uint32 _Value);
    uint32 getEndPosition();
    uint8 getOutputSolarState();


  private:
    uint16 getMinutes(uint8 * _Time);
    bool checkTime(uint8 * _Time); //Return true if it is OK
    ProjectConfig * Settings;
    uint16 counterFailure;
    unsigned long OutputSolarLastChange;
    const unsigned long OutputSolarChangeLock;
    uint8 OutputSolarState;  
    uint32 targetPosition;
    uint8 AutoPositioningOn;
};

#include "Project_Structures.cpp"
#endif