#include "MQTT_Functions.h"
#include "GlobalVariabels.h"
#include "ProjectFunctions.h"

//MQTT-Funktionen
void MQTT_callback(char* topic, byte* payload, unsigned int length)
{
  String strTopic = topic;
  String strPayload = (char*) payload;
  strPayload = strPayload.substring(0, length);
}
bool MQTTinit()
{
  if(MQTTclient.connected())
    MQTTclient.disconnect();
  IPAddress IPTemp;
  if(IPTemp.fromString(varConfig.MQTT_Server))
  {
    MQTTclient.setServer(IPTemp, varConfig.MQTT_Port);
  }
  else{
    MQTTclient.setServer(varConfig.MQTT_Server, varConfig.MQTT_Port);
  }
  MQTTclient.setCallback(MQTT_callback);
  unsigned long int StartTime = millis();
  while ((millis() < (StartTime + 5000)&&(!MQTTclient.connect(WiFi.macAddress().c_str(), varConfig.MQTT_Username, varConfig.MQTT_Password)))){
    delay(200);
  }
  if(MQTTclient.connected()){
  //  int laenge = strlen(varConfig.MQTT_rootpath);
  //  char SubscribeVar[laenge+15];
  //  sprintf(SubscribeVar, "%s/setOutput", varConfig.MQTT_rootpath);
  //  MQTTclient.subscribe(SubscribeVar);
    return true;
  }
  else
    return false;
}
bool MQTT_sendText(int _MSGType, String Text)
{
  return MQTT_sendMessage(_MSGType, (const uint8_t *)Text.c_str(), Text.length());
}

bool MQTT_sendMessage(int MQTT_MSGType, const uint8_t* MSG, uint8_t len)
{
//  int lenPath = strlen(varConfig.MQTT_rootpath);
//  char strPathVar[lenPath+20];
  return false;
/*
  switch (MQTT_MSGType)
  {
  case MQTT_MSG_Error:
    sprintf(strPathVar, "%s/Error", varConfig.MQTT_rootpath);
    return MQTTclient.publish(strPathVar, MSG, len, false);
  case MQTT_MSG_Waterlevel:
    sprintf(strPathVar, "%s/Waterlevel", varConfig.MQTT_rootpath);
    return MQTTclient.publish(strPathVar, MSG, len, true);
  case MQTT_MSG_currentConsume:
    sprintf(strPathVar, "%s/currentConsume", varConfig.MQTT_rootpath);
    return MQTTclient.publish(strPathVar, MSG, len, true);
  case MQTT_MSG_totalConsume:
    sprintf(strPathVar, "%s/totalConsume", varConfig.MQTT_rootpath);
    return MQTTclient.publish(strPathVar, MSG, len, true);
  case MQTT_MSG_stateOutput:
    sprintf(strPathVar, "%s/stateOutput", varConfig.MQTT_rootpath);
    return MQTTclient.publish(strPathVar, MSG, len, true);
  case MQTT_MSG_CounterValue:
    sprintf(strPathVar, "%s/CounterValue", varConfig.MQTT_rootpath);
    return MQTTclient.publish(strPathVar, MSG, len, true);
  
  default:
    return false;
  }*/
}
bool MQTT_sendMessage(int MQTT_MSGType, int MSG)
{
  return MQTT_sendMessage(MQTT_MSGType, (const uint8_t*) IntToStr(MSG).c_str(), IntToStr(MSG).length());
}
bool MQTT_sendMessage(int MQTT_MSGType, uint8_t MSG)
{
  return MQTT_sendMessage(MQTT_MSGType, (const uint8_t*) IntToStr(MSG).c_str(), IntToStr(MSG).length());
}
bool MQTT_sendMessage(int MQTT_MSGType, uint32_t MSG)
{
  return MQTT_sendMessage(MQTT_MSGType, (const uint8_t*) IntToStr(MSG).c_str(), IntToStr(MSG).length());
}
bool MQTT_sendMessage(int MQTT_MSGType, float MSG)
{
  return MQTT_sendMessage(MQTT_MSGType, (const uint8_t*) IntToStr(MSG).c_str(), IntToStr(MSG).length());
}

