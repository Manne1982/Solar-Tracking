
#include "ProjectFunctions.h"
#include "GlobalVariabels.h"
#include <Arduino.h>


//---------------------------------------------------------------------
//Einstellungen laden und Speichern im EEPROM bzw. Flash
void EinstSpeichern()
{
  unsigned long int Checksumme = 0;
  unsigned char *pointer;
  pointer = (unsigned char *)&varConfig;
  for (unsigned long long i = 0; i < sizeof(varConfig); i++)
    Checksumme += pointer[i];

  Serial.print("Gespeicherte Checksumme: ");
  Serial.println(Checksumme);


  //EEPROM initialisieren
  EEPROM.begin(sizeof(varConfig) + 5 + sizeof(*varProject.getSettings()) + 5 + 1);

  EEPROM.put(1, varConfig);
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
  EEPROMSize = sizeof(varConfig) + 5 + sizeof(*varProject.getSettings()) + 5 + 1;
  EEPROM.begin(EEPROMSize);

  EEPROM.get(1, varConfigTest);
  EEPROM.get(sizeof(varConfigTest) + 1, ChecksummeEEPROM);

  for (unsigned long long i = 0; i < sizeof(varConfigTest); i++)
    Checksumme += pointer[i];
  if ((Checksumme == ChecksummeEEPROM) && (Checksumme != 0))
  {
    EEPROM.get(1, varConfig);
    delay(200);
    EEPROM.end(); // Free RAM copy of structure
  }
  else
  {
    Serial.print("Checksumme: ");
    Serial.print(Checksumme);
    Serial.print("ChecksummeEEPROM: ");
    Serial.println(ChecksummeEEPROM);
    
    Serial.println("Fehler beim Dateneinlesen (Allgemeine Einstellung)");
    EEPROM.end(); // Free RAM copy of structure
    EinstSpeichern();
  }
}
//---------------------------------------------------------------------
//Save and load current Project settings
void SaveProjectData()
{
  unsigned long int Checksumme = 0;
  unsigned char *pointer;
  pointer = (unsigned char *) varProject.getSettings();
  for (unsigned long long i = 0; i < sizeof(*varProject.getSettings()); i++)
    Checksumme += pointer[i];

  Serial.print("Gespeicherte Checksumme (Projektdaten): ");
  Serial.println(Checksumme);

  //EEPROM initialisieren
  EEPROM.begin(sizeof(varConfig) + 5 + sizeof(*varProject.getSettings()) + 5 + 1);

  EEPROM.put(sizeof(varConfig) + 5 + 1, *varProject.getSettings());
  EEPROM.put(sizeof(varConfig) + 5 + sizeof(*varProject.getSettings()) + 1, Checksumme);

  EEPROM.commit(); // Only needed for ESP8266 to get data written
  EEPROM.end();    // Free RAM copy of structure
}
void LoadProjectData()
{
  ProjectConfig varConfigTest;
  unsigned long int Checksumme = 0;
  unsigned long int ChecksummeEEPROM = 0;
  unsigned char *pointer;
  pointer = (unsigned char *)&varConfigTest;
  //EEPROM initialisieren
  unsigned int EEPROMSize;
  EEPROMSize = sizeof(varConfig) + 5 + sizeof(*varProject.getSettings()) + 5 + 1;
  EEPROM.begin(EEPROMSize);

  EEPROM.get(sizeof(varConfig) + 5 + 1, varConfigTest);
  EEPROM.get(sizeof(varConfig) + 5 + sizeof(*varProject.getSettings()) + 1, ChecksummeEEPROM);

  for (unsigned long long i = 0; i < sizeof(varConfigTest); i++)
    Checksumme += pointer[i];
  if ((Checksumme == ChecksummeEEPROM) && (Checksumme != 0))
  {
    EEPROM.get(sizeof(varConfig) + 5 + 1, *varProject.getSettings());
    delay(200);
    EEPROM.end(); // Free RAM copy of structure
  }
  else
  {
    Serial.print("Checksumme: ");
    Serial.print(Checksumme);
    Serial.print("ChecksummeEEPROM: ");
    Serial.println(ChecksummeEEPROM);
    
    Serial.println("Fehler beim Dateneinlesen (Projektdaten)");
    EEPROM.end(); // Free RAM copy of structure
    SaveProjectData();
  }

}
//---------------------------------------------------------------------
//Resetvariable die hochzaehlt bei vorzeitigem Stromverlust um auf Standard-Wert wieder zurueckzustellen.
void ResetVarSpeichern(char Count)
{
  EEPROM.begin(1);

  EEPROM.put(0, Count);

  EEPROM.commit(); // Only needed for ESP8266 to get data written
  EEPROM.end();    // Free RAM copy of structure
}
char ResetVarLesen()
{
  unsigned int EEPROMSize;
  char temp = 0;
  EEPROMSize = 1;
  EEPROM.begin(EEPROMSize);
  EEPROM.get(0, temp);
  delay(200);
  EEPROM.end(); // Free RAM copy of structure
  return temp;
}

void IRAM_ATTR handleInterrupt()
{
  switch(varProject.getOutputSolarState())
  {
    case solWest:
    case solWestRunAfter:
      varProject.incrementCounter();
      break;
    case solEast:
    case solEastRunAfter:
      varProject.decrementCounter();
      break;
    default:
      varProject.incrementCounterFailure();
      break;
  }
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

