#include "MailFunctions.h"

void ConfigMailClient(Session_Config * _Config, MailConfig * _Settings)
{
    _Config->server.host_name = _Settings->SMTP_Server; // for gmx
    _Config->server.port = _Settings->Mail_Port; // 587 for TLS with STARTTLS or 25 (Plain/TLS with STARTTLS) or 465 (SSL)
    _Config->login.email = _Settings->Mail_Username; // set to empty for no SMTP Authentication
    _Config->login.password = _Settings->Mail_Password; // set to empty for no SMTP Authentication
    
    // For client identity, assign invalid string can cause server rejection
    _Config->login.user_domain = _Settings->Mail_UserDomain;  
}

int SendMail(SMTPSession * _Smtp, Session_Config * _Config, MailConfig * _Settings, const char * _SenderName, const char * _Subject, const char * _Text)
{
  ConfigMailClient(_Config, _Settings);
    SMTP_Message message;
    // Set the message headers
    message.sender.name = _SenderName;
    message.sender.email = _Settings->Mail_Username;
    message.subject = _Subject;
    message.addRecipient(_Settings->recipientName, _Settings->recipientMail);
  //  message.addRecipient("name2", "email2");

    // message.addCc("email3");
    // message.addBcc("email4");
    message.text.content = _Text;


    
    // Connect to the server
    _Smtp->connect(_Config);
    delay(200);
    // Start sending Email and close the session
    MailClient.sendMail(_Smtp, &message);

    return _Smtp->errorCode();
}