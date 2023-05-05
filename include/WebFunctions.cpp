#include "WebFunctions.h"
#include "GlobalVariabels.h"
#include "MQTT_Functions.h"
#include "HTML_Var.h"
#include "Project_Structures.h"
#include "ProjectFunctions.h"


void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}
void WebserverRoot(AsyncWebServerRequest *request)
{
  char *Header_neu = new char[(strlen(html_header) + 50 + 50)];
  char *HTMLTemp = new char[(strlen(html_Start) + 200)];
  char *HTMLString = new char[(strlen(html_header) + 50)+(strlen(html_Start) + 100)];
  HTMLTemp[0] = 0;
  sprintf(Header_neu, html_header, varProject.getTimeString().c_str(), varError[varProject.isError()].c_str(), InterruptCounter, PollingCounter);
  sprintf(HTMLTemp, html_Start, Un_Checked[varProject.getAutoStateFlag()].c_str(), varDisabled[varProject.getIsNotInit()].c_str(), 
  varDisabled[varProject.getAutoStateFlag()].c_str(), varDisabled[varProject.getAutoStateFlag()].c_str(), varDisabled[varProject.getAutoStateFlag()].c_str(), 
  varDisabled[varProject.getAutoStateFlag()].c_str(), varDisabled[varProject.getAutoStateFlag()].c_str(), varDisabled[varProject.getAutoStateFlag()].c_str(),
  varProject.getStartPosition(), varProject.getMaxPosition(), varProject.getEndPosition(), 
  varProject.getMaxPosition(), varProject.getMaxPosition(), varProject.getCurrentPosition(), varProject.getCouterFailure(),
  varProject.getTimeStart().c_str(), varProject.getTimeEnd().c_str(), varProject.getTimeTurnBack().c_str(), varProject.getTimeAutoBreak());

  //Zusammenfassen der Einzelstrings
  sprintf(HTMLString, "%s%s", Header_neu, HTMLTemp);
  request->send_P(200, "text/html", HTMLString);
  delete[] HTMLTemp;
  delete[] HTMLString;
  delete[] Header_neu;
}
void WebserverSettings(AsyncWebServerRequest *request)
{
  char *Header_neu = new char[(strlen(html_header) + 50)];
  char *Body_neu = new char[(strlen(html_NWconfig)+750)];
  char *HTMLString = new char[(strlen(html_header) + 50)+(strlen(html_NWconfig)+750)];

  char *pntSelected[5];
  for (int i = 0; i < 5; i++)
    if (i == (varConfig.NW_NTPOffset + 2))
      pntSelected[i] = (char *)varSelected[1].c_str();
    else
      pntSelected[i] = (char *)varSelected[0].c_str();
  sprintf(Header_neu, html_header, varProject.getTimeString().c_str(), varError[varProject.isError()].c_str(), InterruptCounter, PollingCounter);
  sprintf(Body_neu, html_NWconfig, Un_Checked[varConfig.NW_Flags & NW_WiFi_AP].c_str(), varConfig.WLAN_SSID, 
              Un_Checked[(varConfig.NW_Flags & NW_StaticIP)/NW_StaticIP].c_str(), varConfig.NW_IPAddress, varConfig.NW_NetzName, varConfig.NW_SubMask, varConfig.NW_Gateway, varConfig.NW_DNS, 
              varConfig.NW_NTPServer, pntSelected[0], pntSelected[1], pntSelected[2], pntSelected[3], pntSelected[4], 
              Un_Checked[(varConfig.NW_Flags & NW_MQTTActive)/NW_MQTTActive].c_str(), varConfig.MQTT_Server, varConfig.MQTT_Port, varConfig.MQTT_Username, varConfig.MQTT_rootpath, Un_Checked[(varConfig.NW_Flags & NW_MQTTSecure)/NW_MQTTSecure].c_str());
  sprintf(HTMLString, "%s%s", Header_neu, Body_neu);
//  request->send(200, "text/html", HTMLString);

  AsyncBasicResponse *response = new AsyncBasicResponse(200, "text/html", HTMLString);
  request->send(response);

  delete[] HTMLString;
  delete[] Body_neu;
  delete[] Header_neu;
}
void WebserverPOST(AsyncWebServerRequest *request)
{
  int parameter = request->params();
  unsigned short int *submitBereich;
  if (parameter)
  {
    submitBereich = (unsigned short int *)request->getParam(0)->name().c_str();
    switch (*submitBereich)
    {
    case subwl:
      varConfig.NW_Flags &= ~NW_WiFi_AP;
      if(parameter <= 3)
      for (int i = 0; i < parameter; i++)
      {
        if (request->getParam(i)->name() == "wlAP")
          varConfig.NW_Flags |= NW_WiFi_AP;
        else if (request->getParam(i)->name() == "wlSSID")
        {
          if (request->getParam(i)->value().length() < 40)
            strcpy(varConfig.WLAN_SSID, request->getParam(i)->value().c_str());
          else
          {
            request->send_P(200, "text/html", "SSID zu lang<form> <input type=\"button\" value=\"Go back!\" onclick=\"history.back()\"></form>");
            return;
          }
        }
        else if (request->getParam(i)->name() == "wlPassword")
        {
          if(request->getParam(i)->value()!="xxxxxx")
          {
            if ((request->getParam(i)->value().length() <= 65)&&(request->getParam(i)->value().length() >= 8))
            { 
              strcpy(varConfig.WLAN_Password, request->getParam(i)->value().c_str());
            }
            else
            {
              request->send_P(200, "text/html", "Passwortlaenge muss zwischen 8 und 65 Zeichen liegen<form> <input type=\"button\" value=\"Go back!\" onclick=\"history.back()\"></form>");
              return;
            }
          }
        }
        else
        {
          request->send_P(200, "text/html", "Unbekannter Rueckgabewert<form> <input type=\"button\" value=\"Go back!\" onclick=\"history.back()\"></form>");
          return;
        }
      }
      request->send_P(200, "text/html", "Daten wurden uebernommen und ESP wird neu gestartet!<br><a href=\\Settings\\>Zurueck</a> <meta http-equiv=\"refresh\" content=\"20; URL=\\\">"); //<a href=\>Startseite</a>
      EinstSpeichern();
      ESP_Restart = true;
      break;
    case subnw:
    {
      char tmp_StatischeIP = 0;
      char tmp_EthernetOn = 0;
      String tmp_IPAddress[5];
      String tmp_NTPServer;
      String tmp_NetzName;
      int tmp_NTPOffset;
      if(parameter < 11)
      for (int i = 0; i < parameter; i++)
      {
        if (request->getParam(i)->name() == "nwSIP")
          tmp_StatischeIP = 1;
        else if (request->getParam(i)->name() == "nwIP")
          tmp_IPAddress[0] = request->getParam(i)->value();
        else if (request->getParam(i)->name() == "nwNetzName")
          tmp_NetzName = request->getParam(i)->value();
        else if (request->getParam(i)->name() == "nwSubnet")
          tmp_IPAddress[2] = request->getParam(i)->value();
        else if (request->getParam(i)->name() == "nwGateway")
          tmp_IPAddress[3] = request->getParam(i)->value();
        else if (request->getParam(i)->name() == "nwDNS")
          tmp_IPAddress[4] = request->getParam(i)->value();
        else if (request->getParam(i)->name() == "nwNTPServer")
          tmp_NTPServer = request->getParam(i)->value();
        else if (request->getParam(i)->name() == "nwNTPOffset")
          sscanf(request->getParam(i)->value().c_str(), "%d", &tmp_NTPOffset);
        else
        {
          request->send_P(200, "text/html", "Unbekannter Rueckgabewert<form> <input type=\"button\" value=\"Go back!\" onclick=\"history.back()\"></form>");
          return;
        }
      }
      if (tmp_StatischeIP)
        if ((tmp_IPAddress[0].length() == 0) || (tmp_IPAddress[1].length() == 0))
        {
          request->send_P(200, "text/html", "Bei Statischer IP-Adresse wird eine IP-Adresse und eine Subnet-Mask benoetigt!<form> <input type=\"button\" value=\"Go back!\" onclick=\"history.back()\"></form>");
          return;
        }
      if(tmp_StatischeIP){
        varConfig.NW_Flags |= NW_StaticIP;}
      else{
        varConfig.NW_Flags &= ~NW_StaticIP;}
      if(tmp_EthernetOn){
        varConfig.NW_Flags |= NW_EthernetActive;}
      else{
        varConfig.NW_Flags &= ~NW_EthernetActive;}
      strcpy(varConfig.NW_IPAddress, tmp_IPAddress[0].c_str());
      strcpy(varConfig.NW_NetzName, tmp_NetzName.c_str());
      strcpy(varConfig.NW_SubMask, tmp_IPAddress[2].c_str());
      strcpy(varConfig.NW_Gateway, tmp_IPAddress[3].c_str());
      strcpy(varConfig.NW_DNS, tmp_IPAddress[4].c_str());
      strcpy(varConfig.NW_NTPServer, tmp_NTPServer.c_str());
      varConfig.NW_NTPOffset = tmp_NTPOffset;
      request->send_P(200, "text/html", "Daten wurden uebernommen und ESP wird neu gestartet!<br><meta http-equiv=\"refresh\" content=\"20; URL=\\\">"); //<a href=\>Startseite</a>
      EinstSpeichern();
      ESP_Restart = true;
      break;
    }
    case submq:
    {
      char tmp_MQTTOn = 0, tmp_MQTTSecureOn = 0;
      String Temp[6];
      for (int i = 0; i < parameter; i++)
      {
        if (request->getParam(i)->name() == "mqMQTTOn")
          tmp_MQTTOn = 1;
        else if (request->getParam(i)->name() == "mqServer")
          Temp[0] = request->getParam(i)->value();
        else if (request->getParam(i)->name() == "mqPort")
          Temp[1] = request->getParam(i)->value();
        else if (request->getParam(i)->name() == "mqUser")
          Temp[2] = request->getParam(i)->value();
        else if (request->getParam(i)->name() == "mqPassword")
          Temp[3] = request->getParam(i)->value();
        else if (request->getParam(i)->name() == "mqRootpath")
          Temp[4] = request->getParam(i)->value();
        else if (request->getParam(i)->name() == "mqMQTTsecureOn")
          tmp_MQTTSecureOn = 1;
        else if (request->getParam(i)->name() == "mqFPrint")
          Temp[5] = request->getParam(i)->value();
        else
        {
          request->send_P(200, "text/html", "Unbekannter Rueckgabewert<form> <input type=\"button\" value=\"Go back!\" onclick=\"history.back()\"></form>");
          return;
        }
      }
      if(tmp_MQTTOn){
        varConfig.NW_Flags |= NW_MQTTActive;}
      else{
        varConfig.NW_Flags &= ~NW_MQTTActive;}
      if(tmp_MQTTSecureOn){
        varConfig.NW_Flags |= NW_MQTTSecure;}
      else{
        varConfig.NW_Flags &= ~NW_MQTTSecure;}
      if((Temp[0].length()<49)&&(Temp[0].length()>5))
        strcpy(varConfig.MQTT_Server, Temp[0].c_str());
      if((Temp[1].length()<6)&&(Temp[1].length()>1))
        varConfig.MQTT_Port = Temp[1].toInt();
      if((Temp[2].length()<19)&&(Temp[2].length()>5))
        strcpy(varConfig.MQTT_Username, Temp[2].c_str());
      if((Temp[3].length()<=60)&&(Temp[3].length()>=5)&&(Temp[3]!= "xxxxxx"))
        strcpy(varConfig.MQTT_Password, Temp[3].c_str());
      if((Temp[4].length()<95)&&(Temp[4].length()>5))
        strcpy(varConfig.MQTT_rootpath, Temp[4].c_str());
      if((Temp[5].length()<=65)&&(Temp[5].length()>5)&&(Temp[5]!= "xxxxxx"))
        strcpy(varConfig.MQTT_fprint, Temp[5].c_str());
      EinstSpeichern();
      request->send_P(200, "text/html", "MQTT Daten wurden uebernommen, ESP startet neu!<br><meta http-equiv=\"refresh\" content=\"20; URL=\\\">"); //<a href=\>Startseite</a>
      ESP_Restart = true;
      break;
    }
    case subcn:
    {
      uint16 CheckboxSet = 0;
      if(parameter <= 2)
      for (int i = 0; i < parameter; i++)
      {
        uint8 Temp = 0;
        if(sscanf(request->getParam(i)->name().c_str(), "cnt%hhu", &Temp) == 1)
        {
          switch (Temp)
          {
          case 11:
            if(varProject.isError())
            {
              request->send_P(200, "text/html", "Fehler, Automatic Betrieb konnte nicht aktiviert werden!<br><meta http-equiv=\"refresh\" content=\"5; URL=\\\">");
              break;
            }
            if(CheckboxSet == 12)
            {
              varProject.startAutoMode();
              varProject.goToAutoPosition();
              request->send_P(200, "text/html", "Automatic Betrieb aktiviert!<br><meta http-equiv=\"refresh\" content=\"2; URL=\\\">");
            }
            else
            {
              varProject.stopAutoMode();
              request->send_P(200, "text/html", "Automatic Betrieb deaktiviert!<br><meta http-equiv=\"refresh\" content=\"2; URL=\\\">");
            }
            break;
          case 12:
            CheckboxSet = 12;
            break;
          case 21:
            if(varProject.getAutoStateFlag())
              break;
            if(varProject.isError())
            {
              request->send_P(200, "text/html", "Fehler, Referenzfahrt kann nicht gestartet werden!<br><meta http-equiv=\"refresh\" content=\"5; URL=\\\">");
              break;
            }
            if(CheckboxSet == 22)
            {
              varProject.StartReference();
              request->send_P(200, "text/html", "Referenzfahrt gestartet!<br><meta http-equiv=\"refresh\" content=\"2; URL=\\\">");
            }
            else
            {
              if(varProject.getReferenceState())
              {
                varProject.AbortReference();
                request->send_P(200, "text/html", "Referenzfahrt abgebrochen!<br><meta http-equiv=\"refresh\" content=\"2; URL=\\\">");
              }
              else
              {
                request->send_P(200, "text/html", "Referenzfahrt ist nicht gestartet!<br><meta http-equiv=\"refresh\" content=\"2; URL=\\\">");
              }
            }
            break;
          case 22:
            CheckboxSet = 22;
            break;
          case 31:
            if(varProject.getAutoStateFlag())
              break;
            if(varProject.isError())
            {
              request->send_P(200, "text/html", "Fehler, Motor kann nicht gestartet werden!<br><meta http-equiv=\"refresh\" content=\"5; URL=\\\">");
              break;
            }
            varProject.TurnSolar(solWest);
            request->send_P(200, "text/html", "Solar West!<br><meta http-equiv=\"refresh\" content=\"0; URL=\\\">");
            break;
          case 71:
            if(varProject.getAutoStateFlag())
              break;
            if(varProject.isError())
            {
              request->send_P(200, "text/html", "Fehler, Motor kann nicht gestartet werden!<br><meta http-equiv=\"refresh\" content=\"5; URL=\\\">");
              break;
            }
            varProject.goToTime(solWest, 1000);
            request->send_P(200, "text/html", "Solar West!<br><meta http-equiv=\"refresh\" content=\"0; URL=\\\">");
            break;
          case 41:
            if(varProject.getAutoStateFlag())
              break;
            varProject.TurnSolar(solOff);
            request->send_P(200, "text/html", "Solar off!<br><meta http-equiv=\"refresh\" content=\"0; URL=\\\">");
            break;
          case 51:
            if(varProject.getAutoStateFlag())
              break;
            if(varProject.isError())
            {
              request->send_P(200, "text/html", "Fehler, Motor kann nicht gestartet werden!<br><meta http-equiv=\"refresh\" content=\"5; URL=\\\">");
              break;
            }
            varProject.TurnSolar(solEast);
            request->send_P(200, "text/html", "Solar East!<br><meta http-equiv=\"refresh\" content=\"0; URL=\\\">");
            break;
          case 81:
            if(varProject.getAutoStateFlag())
              break;
            if(varProject.isError())
            {
              request->send_P(200, "text/html", "Fehler, Motor kann nicht gestartet werden!<br><meta http-equiv=\"refresh\" content=\"5; URL=\\\">");
              break;
            }
            varProject.goToTime(solEast, 1000);
            request->send_P(200, "text/html", "Solar East!<br><meta http-equiv=\"refresh\" content=\"0; URL=\\\">");
            break;
          case 61:
            varProject.resetErrorFlag();
            SaveProjectData();
            request->send_P(200, "text/html", "Fehler zurueckgesetzt!<br><meta http-equiv=\"refresh\" content=\"2; URL=\\\">");
            break;
          
          default:
            break;
          }
        }
      }
      break;
    }
    case subtm:
    {
      String ZeitenTemp[4];
      uint16 TempMinuten[4] = {0, 0, 0, 0};
      if(parameter == 4)
      {
        for (int i = 0; i < parameter; i++)
        {
          uint8 Temp = 0;
          if(sscanf(request->getParam(i)->name().c_str(), "tm%hhu", &Temp) == 1)
          {
            switch (Temp)
            {
            case 1://StartTime
            case 2://EndTime
            case 3://TurnBack
              ZeitenTemp[Temp-1]= request->getParam(Temp-1)->value();
              TempMinuten[Temp-1] = varProject.getMinutes(request->getParam(Temp-1)->value());
              break;
            case 4:
              TempMinuten[3] = request->getParam(3)->value().toInt();
              break;
            default:
              request->send_P(200, "text/html", "Falscher Parametername!<br><meta http-equiv=\"refresh\" content=\"2; URL=\\\">");
              return;
            }
          }
        }
      }
      else
      {
        request->send_P(200, "text/html", "Parameteranzahl falsch!<br><meta http-equiv=\"refresh\" content=\"2; URL=\\\">");
        return;
      }
      if((TempMinuten[0] < TempMinuten[1])&&((TempMinuten[2]<TempMinuten[0])||(TempMinuten[2]>TempMinuten[1]))&&((TempMinuten[1]-TempMinuten[0])>(5*TempMinuten[3])))
      {
        varProject.setTime(ZeitenTemp[0], ZeitenTemp[1], ZeitenTemp[2]);
        varProject.setTimeAutoBreak(TempMinuten[3]);
        request->send_P(200, "text/html", "Neue Zeiteinstellung wurde uebernommen!<br><meta http-equiv=\"refresh\" content=\"2; URL=\\\">");
        SaveProjectData();
      }
      else
      {
        request->send_P(200, "text/html", "Fehler, neue Zeiteinstellung nicht uebernommen!<br><meta http-equiv=\"refresh\" content=\"2; URL=\\\">");
      }
      break;
    }
    case subpo:
    {
      uint32 TempPositions[2] = {0, 0};
      if(parameter == 2)
      {
        for (int i = 0; i < parameter; i++)
        {
          uint8 Temp = 0;
          if(sscanf(request->getParam(i)->name().c_str(), "pos%hhu", &Temp) == 1)
          {
            if((Temp == 1)||(Temp == 2))  
              TempPositions[Temp-1]= request->getParam(Temp-1)->value().toInt();
            else
              request->send_P(200, "text/html", "Parametername unbekannt!<br><meta http-equiv=\"refresh\" content=\"2; URL=\\\">");
          }
        }
      }
      else
      {
        request->send_P(200, "text/html", "Parameteranzahl falsch!<br><meta http-equiv=\"refresh\" content=\"2; URL=\\\">");
        return;
      }
      if((TempPositions[0] < TempPositions[1])&&(TempPositions[1]<=varProject.getMaxPosition()))
      {
        varProject.setStartPosition(TempPositions[0]);
        varProject.setEndPosition(TempPositions[1]);
        request->send_P(200, "text/html", "Neue Positionseinstellung wurde uebernommen!<br><meta http-equiv=\"refresh\" content=\"2; URL=\\\">");
        SaveProjectData();
      }
      else
      {
        request->send_P(200, "text/html", "Fehler, neue Positionseinstellung nicht uebernommen!<br><meta http-equiv=\"refresh\" content=\"2; URL=\\\">");
      }
      break;
    }
    default:
      char strFailure[50];
      sprintf(strFailure, "Anweisung unbekannt, Empfangen: %u", *submitBereich);
      request->send_P(200, "text/html", strFailure);
      break;
    }
  }
}
