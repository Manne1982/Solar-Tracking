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
  char ResetCount;
  varProject.InitIO();
  attachInterrupt(digitalPinToInterrupt(CounterPinPosition), handleInterrupt, FALLING);

  ResetCount = ResetVarLesen();
  if((ResetCount < 0)||(ResetCount > 5))  //Prüfen ob Wert Plausibel, wenn nicht rücksetzen
    ResetCount = 0;

    //ResetCount++;
  ResetVarSpeichern(ResetCount);
  delay(1000);
  if (ResetCount < 5) //Wenn nicht 5 mal in den ersten 5 Sekunden der Startvorgang abgebrochen wurde
  {
    EinstLaden();
    LoadProjectData();
    ResetVarSpeichern(0);
  }
  //WLAN starten
  if (varConfig.NW_Flags&NW_WiFi_AP)
    WiFi_Start_AP(varConfig.WLAN_SSID, varConfig.WLAN_Password);
  else
    WiFi_Start_STA(varConfig.WLAN_SSID, varConfig.WLAN_Password);

  //Zeitserver Einstellungen
  if (strlen(varConfig.NW_NTPServer))
    timeClient = new NTPClient(ntpUDP, (const char *)varConfig.NW_NTPServer);
  else
    timeClient = new NTPClient(ntpUDP, "fritz.box");
  delay(1000);

  timeClient->begin();
  timeClient->setTimeOffset(varConfig.NW_NTPOffset * 3600);

  //MQTT
  MQTTinit();

  //OTA
  ArduinoOTA.setHostname("SolarTracking");
  ArduinoOTA.setPassword("SolarTracking!123");
  ArduinoOTA.begin();
  for(int i = 0; i<10; i++)
  {
    //OTA
    ArduinoOTA.handle();
    delay(1000);
  }

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
    timeClient->update();
  }
    //Anweisungen werden alle 10 Minuten ausgefuehrt
  if (Break_10m < millis())
  {
    Break_10m = millis() + 600000;
    WIFIConnectionCheck(true);
  }
  //Anweisungen werden alle 20 Sekunden ausgefuehrt
  if (Break_60s < millis())
  {
    Break_60s = millis() + 60000;
    //Vorbereitung Datum
    unsigned long epochTime = timeClient->getEpochTime();
    struct tm *ptm = gmtime((time_t *)&epochTime);
    monthDay = ptm->tm_mday;
    currentMonth = ptm->tm_mon + 1;
    currentYear = ptm->tm_year + 1900;
    currentHour = ptm->tm_hour;
    currentMin = ptm->tm_min;
    
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
  if (Break_1s < millis())
  {
    Break_1s = millis() + 1000;
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
}


//---------------------------------------------------------------------
