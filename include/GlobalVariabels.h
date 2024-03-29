#ifndef GlobalVariabels
#define GlobalVariabels

#include <Arduino.h>
#include "ESPAsyncWebServer.h"
#include "Project_Structures.h"
#include <PubSubClient.h>


#define BGTDEBUG 1

//Projektvariablen
NWConfig varConfig;
ProjectClass varProject;


//-----------------------------------
//Touch-Variablen neu
unsigned long Break_200ms = 0;                             //Variable fuer Dinge die alle 200ms ausgefuehrt werden
unsigned long Break_10s = 0;                                //Variable fuer Dinge die alle 10s ausgefuehrt werden
unsigned long Break_60s = 0;                               //Variable fuer Dinge die alle 60s ausgefuehrt werden
unsigned long Break_10m = 0;                               //Variable fuer Dinge die alle 10m ausgefuehrt werden
unsigned long Break_h = 0;                                 //Variable fuer Dinge die alle 1h ausgefuehrt werden
unsigned long InterruptCounter = 0;
unsigned long InterruptCounterOld = 0;
unsigned long PollingCounter = 0;
unsigned long CountBreakInt = 0;
unsigned long CountBreakPol = 0;
uint8 PollingLastState = 0;
char MQTTState[20] = "MQTT Ready";
char WifiState[20] = "Wifi Ready";
bool ESP_Restart = false;


//Erstellen Serverelement
AsyncWebServer server(8080);


//MQTT Variablen
WiFiClient wifiClient;
PubSubClient MQTTclient(wifiClient);

#endif