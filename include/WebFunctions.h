#ifndef WebFunctions
#define WebFunctions
#include "ESPAsyncWebServer.h"

void notFound(AsyncWebServerRequest *request);
void WebserverRoot(AsyncWebServerRequest *request);
void WebserverSettings(AsyncWebServerRequest *request);
void WebserverMail(AsyncWebServerRequest *request);
void WebserverPOST(AsyncWebServerRequest *request);
void WebserverViewLog(AsyncWebServerRequest *request);

char * GetLastMessagesHTML();


#include "WebFunctions.cpp"
#endif