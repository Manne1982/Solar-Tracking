//#include "Farben.h"
#include <Arduino.h>
#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "ESPAsyncWebServer.h"
#include <EEPROM.h>
#include <ArduinoOTA.h>
//Projektspezifisch
#include "HTML_Var.h"
#include "WebFunctions.h"
#include "GlobalVariabels.h"
#include "MQTT_Functions.h"
#include "WiFiFunctions.h"
#include "MailFunctions.h"

void setup(void)
{
  uint8 ResetCount;
  varProject.InitIO();
  varProject.varNWConfig = &varConfig;
  attachInterrupt(digitalPinToInterrupt(CounterPinPosition), handleInterrupt, FALLING);
  Serial.begin(9600);
  delay(1000);
  Serial.println("Neustart");
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
    LoadMailConfig();
    ConfigMailClient(&varProject.Mail_config, varProject.MailSettings);
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
  server.on("/Mail", HTTP_GET, WebserverMail);
  server.on("/POST", HTTP_POST, WebserverPOST);
  server.on("/Log", HTTP_GET, WebserverViewLog);

//  ConfigMailClient();
//  SendMail();
}

void loop()
{
  
  //OTA
  ArduinoOTA.handle();
  if(varProject.getOutputSolarState()==solOff)
  {
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
    /*if (Break_200ms < millis())
    {
      Break_200ms = millis() + 200;
    }*/

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
  //Restart for WWW-Requests
  if(ESP_Restart)
  {
    delay(1000);
    ESP.restart();
  }
  if((delete_Response > 0) && (delete_Response < millis()))
  {
    delete_Response = 0;
    delete response;
    response = 0;
  }
  if(boolSendMail)
  {
    SendMail(&varProject.smtp, &varProject.Mail_config, varProject.MailSettings, varConfig.NW_NetzName, msgSubject, msgText);
    boolSendMail = false;
    delete[] msgSubject;
    msgSubject = 0;
    delete[] msgText;
    msgText = 0;

  }
  if(digitalRead(CounterPinPosition))
  {
   // PollingCounter += (PollingLastState + 1)%2;
    PollingLastState = 1;
  }
  else
  {
    if(CountBreakPol < millis())
    {
      PollingCounter += (PollingLastState * 1);
      PollingLastState = 0;
      CountBreakPol = millis() + 100;
    }
  }
  varProject.loop(&PollingCounter, &InterruptCounterOld);
}


//---------------------------------------------------------------------
