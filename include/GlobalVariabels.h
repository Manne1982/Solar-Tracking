#ifndef GlobalVariabels
#define GlobalVariabels

#include <Arduino.h>
#include "ESPAsyncWebServer.h"
#include "Project_Structures.h"
#include <NTPClient.h>


//Projektvariablen
NWConfig varConfig;
//-----------------------------------
//Touch-Variablen neu
unsigned long Break_200ms = 0;                             //Variable fuer Dinge die alle 200ms ausgefuehrt werden
unsigned long Break_1s = 0;                                //Variable fuer Dinge die alle 1s ausgefuehrt werden
unsigned long Break_60s = 0;                               //Variable fuer Dinge die alle 60s ausgefuehrt werden
unsigned long Break_10m = 0;                               //Variable fuer Dinge die alle 10m ausgefuehrt werden
unsigned long Break_h = 0;                                 //Variable fuer Dinge die alle 1h ausgefuehrt werden
int monthDay = 0;                                          //Variablen für die aktuelle Zeit
int currentMonth = 0;
int currentYear = 0;
int currentHour = 0;
int currentMin = 0;
char MQTTState[20] = "MQTT Ready";
char WifiState[20] = "Wifi Ready";
char WifiLastChange[50] = "";         //Aufnahme des Zeitstempels wann letztes mal die Verbindung neu aufgebaut wurde inklusive des WIFI Statuses
bool ESP_Restart = false;

//Erstellen Serverelement
AsyncWebServer server(8080);

//Uhrzeit Variablen
WiFiUDP ntpUDP;
NTPClient *timeClient;

//MQTT Variablen
WiFiClient wifiClient;
PubSubClient MQTTclient(wifiClient);

#endif