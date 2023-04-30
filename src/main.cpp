//#include "Farben.h"
#include <Arduino.h>
#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "ESPAsyncWebServer.h"
#include <EEPROM.h>
//MQTT
#include <PubSubClient.h>
//fuer Uhrzeitabruf notwendig
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
//Projektspezifisch
#include "HTML_Var.h"
#include "WebFunctions.h"
#include "GlobalVariabels.h"
#include "MQTT_Functions.h"
#include "WiFiFunctions.h"



void setup(void)
{
  uint8 ResetCount;
  Serial.begin(9600);
  varProject.InitIO();
  attachInterrupt(digitalPinToInterrupt(CounterPinPosition), handleInterrupt, FALLING);
 
  ResetCount = ResetVarLesen();
  if(ResetCount > 10)  //Prüfen ob Wert Plausibel, wenn nicht rücksetzen
  {
    ResetCount = 0;
    ResetVarSpeichern(ResetCount);
  }
  else
    ResetCount++;
  //OTA
  ArduinoOTA.setHostname("SolarTracker");
  ArduinoOTA.setPassword("SolarTracker!123");
  ArduinoOTA.begin();
  for(int i = 0; i<10; i++)
  {
    //OTA
    ArduinoOTA.handle();
    delay(1000);
  }
//  if (ResetCount < 5) //Wenn nicht 5 mal in den ersten 10 Sekunden der Startvorgang abgebrochen wurde
//  {
    EinstLaden();
    LoadProjectData();
    ResetVarSpeichern(0);
//  }
  //EinstSpeichern();
  //WLAN starten
  if (varConfig.NW_Flags&NW_WiFi_AP)
    WiFi_Start_AP(varConfig.WLAN_SSID, varConfig.WLAN_Password);
  else
    WiFi_Start_STA(varConfig.WLAN_SSID, varConfig.WLAN_Password);
  
  delay(1000);

  //Zeitserver Einstellungen
  if (strlen(varConfig.NW_NTPServer))
    varProject.InitTimeClient(varConfig.NW_NTPServer, varConfig.NW_NTPOffset * 3600);
  else
    varProject.InitTimeClient("fritz.box", varConfig.NW_NTPOffset * 3600);

  //MQTT
  MQTTinit();


  //Webserver
  server.onNotFound(notFound);
  server.begin();
  server.on("/", HTTP_GET, WebserverRoot);
  server.on("/Settings", HTTP_GET, WebserverSettings);
  server.on("/POST", HTTP_POST, WebserverPOST);

  //MQTT_sendText(MQTT_MSG_Logging, "Water control rebooted!");
}

void loop()
{
  
  //OTA
  ArduinoOTA.handle();
  //Anweisungen werden alle 3600 Sekunden (1h) ausgefuehrt
  if (Break_h < millis())
  {
    Break_h = millis() + 3600000;
    varProject.timeClientUpdate();
  }
    //Anweisungen werden alle 10 Minuten ausgefuehrt
  if (Break_10m < millis())
  {
    Break_10m = millis() + 600000;
    if((varProject.getOutputSolarState() == solOff) && !varProject.anyChange())
      SaveProjectData();    
    WIFIConnectionCheck(true);
  }
  //Anweisungen werden alle 20 Sekunden ausgefuehrt
  if (Break_60s < millis())
  {
    Break_60s = millis() + 60000;
    if((varProject.getOutputSolarState() == solOff) && varProject.anyChange())
      SaveProjectData();    

    //MQTT Verbindungskontrolle und neu verbinden
    if ((MQTTclient.state() != 0)&&(varConfig.NW_Flags&NW_MQTTActive))
    {
      if(WIFIConnectionCheck(false))
      {
        WifiState[0] = 'W';
        MQTTclient.disconnect();
        MQTTinit();
      }
      else
        WifiState[0] = 0;
    }
    else
    {
      sprintf(MQTTState, "MQTT Ready");
      WifiState[0] = 'W';
    }
  }

  //Anweisungen werden alle 1 Sekunden ausgefuehrt
  if (Break_10s < millis())
  {
    Break_10s = millis() + 10000;
    //Vorbereitung Datum
    varProject.checkSchedule();
  }

  //Anweisungen werden alle 200 Millisekunden ausgefuehrt
  if (Break_200ms < millis())
  {
    Break_200ms = millis() + 200;
  }

  //MQTT wichtige Funktion
  if((WiFi.status()== WL_CONNECTED)&&(varConfig.NW_Flags&NW_MQTTActive))  //MQTTclient.loop() wichtig damit die Daten im Hintergrund empfangen werden
  {
    if(!MQTTclient.loop())
    {
      MQTTclient.disconnect();
      MQTTinit();
    }
  }
  //Restart for WWW-Requests
  if(ESP_Restart)
  {
    delay(1000);
    ESP.restart();
  }

  varProject.loop();
}


//---------------------------------------------------------------------
