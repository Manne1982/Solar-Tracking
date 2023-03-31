#ifndef ProjectFunctions
#define ProjectFunctions
#include <Arduino.h>
#include "ProjectFunctions.h"


void EinstSpeichern();
void EinstLaden();
char ResetVarLesen();
void ResetVarSpeichern(char Count);


String IntToStr(int _var);
String IntToStr(float _var);
String IntToStrHex(int _var);
String IntToStr(uint32_t _var);



#include "ProjectFunctions.cpp"
#endif