
#include "ProjectFunctions.h"
#include "GlobalVariabels.h"


//---------------------------------------------------------------------
//Einstellungen laden und Speichern im EEPROM bzw. Flash
void EinstSpeichern()
{
  unsigned long int Checksumme = 0;
  unsigned char *pointer;
  pointer = (unsigned char *)&varConfig;
  for (unsigned long long i = 0; i < sizeof(varConfig); i++)
    Checksumme += pointer[i];

  //EEPROM initialisieren
  EEPROM.begin(sizeof(varConfig) + 14);

  EEPROM.put(0, varConfig);
  EEPROM.put(sizeof(varConfig) + 1, Checksumme);

  EEPROM.commit(); // Only needed for ESP8266 to get data written
  EEPROM.end();    // Free RAM copy of structure
}
void EinstLaden()
{
  NWConfig varConfigTest;
  unsigned long int Checksumme = 0;
  unsigned long int ChecksummeEEPROM = 0;
  unsigned char *pointer;
  pointer = (unsigned char *)&varConfigTest;
  //EEPROM initialisieren
  unsigned int EEPROMSize;
  EEPROMSize = sizeof(varConfig) + 14;
  EEPROM.begin(EEPROMSize);

  EEPROM.get(0, varConfigTest);
  EEPROM.get(sizeof(varConfigTest) + 1, ChecksummeEEPROM);

  for (unsigned long long i = 0; i < sizeof(varConfigTest); i++)
    Checksumme += pointer[i];
  if ((Checksumme == ChecksummeEEPROM) && (Checksumme != 0))
  {
    EEPROM.get(0, varConfig);
  }
  else
    Serial.println("Fehler beim Dateneinlesen");

  delay(200);
  EEPROM.end(); // Free RAM copy of structure
}
//---------------------------------------------------------------------
//Resetvariable die hochzaehlt bei vorzeitigem Stromverlust um auf Standard-Wert wieder zurueckzustellen.
void ResetVarSpeichern(char Count)
{
  EEPROM.begin(sizeof(varConfig) + 14);

  EEPROM.put(sizeof(varConfig) + 10, Count);

  EEPROM.commit(); // Only needed for ESP8266 to get data written
  EEPROM.end();    // Free RAM copy of structure
}
char ResetVarLesen()
{
  unsigned int EEPROMSize;
  char temp = 0;
  EEPROMSize = sizeof(varConfig) + 14;
  EEPROM.begin(EEPROMSize);
  EEPROM.get(EEPROMSize - 4, temp);
  delay(200);
  EEPROM.end(); // Free RAM copy of structure
  return temp;
}

String IntToStr(int _var)
{
    char Temp[20];
    sprintf(Temp,"%d",_var);
    return Temp;
}
String IntToStr(char _var)
{
    char Temp[20];
    sprintf(Temp,"%d",_var);
    return Temp;
}
String IntToStr(long int _var)
{
    char Temp[20];
    sprintf(Temp,"%ld",_var);
    return Temp;
}
String IntToStr(uint32_t _var)
{
    char Temp[20];
    sprintf(Temp,"%u",_var);
    return Temp;
}
String IntToStr(float _var)
{
    char Temp[20];
    sprintf(Temp,"%f",_var);
    return Temp;
}
String IntToStrHex(int _var)
{
    char Temp[20];
    sprintf(Temp,"%x",_var);
    return Temp;
}

