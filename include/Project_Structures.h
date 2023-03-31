#ifndef Project_Structures
#define Project_Structures
#include <Arduino.h>




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
struct ProjectConfig {
  uint8 Flags = 0; 
  uint32 MaxPosition;
  uint32 CurrentPosition;
  uint32 StartPosition;
  uint32 EndPositon;
  uint8 StartTime[2];
  uint8 EndTime[2];
  uint16 BreakMinute;
};

#include "Project_Structures.cpp"
#endif