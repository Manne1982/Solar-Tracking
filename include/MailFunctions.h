#ifndef MailFunctions
#define MailFunctions
#include <ESP_Mail_Client.h>
#include <Arduino.h>
enum {Mail_NotifyOn = 0x0001, Mail_NotifyAutoOff = 0x0002, Mail_DailyTest = 0x0004};

struct MailConfig{
  uint16 Flags = 0; 
  char SMTP_Server[20] = "";
  uint16 Mail_Port = 465;
  char Mail_Username[50] = "Benutzer@gmail.com";
  char Mail_Password[40] = "123456";
  char Mail_UserDomain[100] = "";
  char recipientName[30] = "";
  char recipientMail[50] = "";
};


void ConfigMailClient(Session_Config * _Config, MailConfig * _Settings);
int SendMail(SMTPSession * _Smtp, Session_Config * _Config, MailConfig * _Settings, const char * _SenderName, const char * _Subject, const char * _Text);



#include "MailFunctions.cpp"
#endif