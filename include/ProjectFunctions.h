#ifndef ProjectFunctions
#define ProjectFunctions
#include <Arduino.h>


void EinstSpeichern();
void EinstLaden();
void SaveProjectData();
void LoadProjectData();
char ResetVarLesen();
void ResetVarSpeichern(char Count);

void IRAM_ATTR handleInterrupt();

String IntToStr(int _var);
String IntToStr(float _var);
String IntToStrHex(int _var);
String IntToStr(uint32_t _var);



#include "ProjectFunctions.cpp"
#endif