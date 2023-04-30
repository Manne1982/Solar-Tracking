#include "Project_Structures.h"
#include "GlobalVariabels.h"


ProjectClass::ProjectClass():
counterFailure(0),
OutputSolarLastChange(0),
OutputSolarChangeLock(5000),
OutputSolarState(0),
OutputSolarStateBefore(0),
targetPosition(0),
AutoPositioningOn(0),
anyPosChange(0),
referenceState(0),
LastPosChange(0),
TimeToStop(0),
LastPosChangeMinutes(0),
monthDay(0),
currentMonth(0),
currentYear(0),
currentHour(0),
currentMin(0)
{
    Settings = new ProjectConfig;
    Settings->TimeEnd[tmHour] = 22;
    Settings->TimeEnd[tmMinute] = 59;
    Settings->TimeStart[tmHour] = 5;
    Settings->TimeStart[tmMinute] = 0;
    Settings->TimeTurnBack[tmHour] = 4;
    Settings->TimeTurnBack[tmHour] = 0;
    ntpUDP = new WiFiUDP;
    timeClient = 0;
}

ProjectClass::~ProjectClass()
{
    delete Settings;
    delete ntpUDP;
}
void ProjectClass::InitIO()
{
  pinMode(OutputTurnOnOff, OUTPUT);
  digitalWrite(OutputTurnOnOff, 1);
  pinMode(OutputTurnDirection, OUTPUT);
  digitalWrite(OutputTurnDirection, 1);
  pinMode(OutputRelais3, OUTPUT);
  digitalWrite(OutputRelais3, 1);
  pinMode(OutputRelais4, OUTPUT);
  digitalWrite(OutputRelais4, 1);

  pinMode(CounterPinPosition, INPUT_PULLUP); 
}
void ProjectClass::InitTimeClient(String _Server, long int _Offset)
{
    delay(1000);
    timeClient = new NTPClient(*ntpUDP, _Server.c_str());
    delay(1000);
    timeClient->begin();
    timeClient->setTimeOffset(_Offset);
    delay(1000);
    timeClient->update();
}
void ProjectClass::timeClientUpdate()
{
    timeClient->update();
}
void ProjectClass::checkSchedule()
{
    unsigned long long epochTime = timeClient->getEpochTime();
    struct tm *ptm = gmtime((time_t *)&epochTime);
    monthDay = ptm->tm_mday;
    currentMonth = ptm->tm_mon + 1;
    currentYear = ptm->tm_year + 1900;
    currentHour = ptm->tm_hour;
    currentMin = ptm->tm_min;
    if(!(Settings->Flags&flagAutoModeOn)||!(Settings->Flags&flagInitialised))
        return;
    uint16 StartMinutes = getMinutes(Settings->TimeStart);
    uint16 currentMinutes = currentHour * 60 + currentMin;
    uint16 EndMinutes = getMinutes(Settings->TimeEnd);
    if((currentMinutes>=getMinutes(Settings->TimeTurnBack))&&((currentMinutes < StartMinutes)||(currentMinutes > EndMinutes))&&(Settings->CurrentPosition > Settings->StartPosition)&&(OutputSolarState==solOff))
    {
        goToStart();
        return;
    }
    if((currentMinutes >= EndMinutes)&&(Settings->CurrentPosition < Settings->EndPosition))
    {
        goToEnd();
    }
    if((StartMinutes > currentMinutes)||(EndMinutes < currentMinutes))
        return;
    uint16 quantityPosChanges = (EndMinutes - StartMinutes)/Settings->BreakMinute;
    uint16 PositionSteps = (Settings->EndPosition - Settings->StartPosition)/quantityPosChanges;
    uint16 start = ((EndMinutes - StartMinutes)%Settings->BreakMinute)/2;

    if(!((currentMinutes + start - StartMinutes)%Settings->BreakMinute)&& (currentMinutes != LastPosChangeMinutes))
    {
        goToPosition(PositionSteps *((currentMinutes + start - StartMinutes)/Settings->BreakMinute));
        LastPosChangeMinutes = currentMinutes;
    }
}
String ProjectClass::getTimeString()
{
    char Temp[100];
    sprintf(Temp, "Uhrzeit: %s | Datum: %s, %02d.%02d.%d", timeClient->getFormattedTime().c_str(), WeekDays[timeClient->getDay()].c_str(), monthDay, currentMonth, currentYear);
    return Temp;
}
void ProjectClass::TurnSolar(uint8 _value)
{
    if(Settings->Flags & flagError)
        _value = solOff;
  switch (_value)
  {
  case solOff:
    digitalWrite(OutputTurnOnOff, 1);
    digitalWrite(OutputTurnDirection, 1);
    OutputSolarLastChange = millis();
    OutputSolarState = solOff;
    break;
  case solWest:
    if(((OutputSolarLastChange + OutputSolarChangeLock)>millis()))
    break;
    if(OutputSolarState == solEast)
    {
      TurnSolar(solOff);
      break;
    }
    digitalWrite(OutputTurnDirection, 1);
    digitalWrite(OutputTurnOnOff, 0);
    OutputSolarLastChange = millis();
    LastPosChange = millis();
    OutputSolarState = solWest;
    OutputSolarStateBefore = solWest;
    break;
  case solEast:
    if(((OutputSolarLastChange + OutputSolarChangeLock)>millis()))
    break;
    if(OutputSolarState == solWest)
    {
      TurnSolar(solOff);
      break;
    }
    digitalWrite(OutputTurnDirection, 0);
    digitalWrite(OutputTurnOnOff, 0);
    OutputSolarLastChange = millis();
    LastPosChange = millis();
    OutputSolarState = solEast;
    OutputSolarStateBefore = solEast;
    break;
  
  default:
    break;
  }
}
void ProjectClass::loop()
{
    if(anyChange())
    {
        LastPosChange = millis();
    }
    if(referenceState)
    {
        ReferenceLoop();
        return;
    }
    if((TimeToStop > 0) && (TimeToStop <= millis()))
        TurnSolar(solOff);

    if(((LastPosChange + 10000) < millis()) && (OutputSolarState!=solOff) && getAutoStateFlag())
    {
        TurnSolar(solOff);
        stopAutoMode();
        Settings->Flags |= flagError;
    }
    if(!AutoPositioningOn)
        return;
    switch (OutputSolarState)
    {
    case solWest:
        if((getCurrentPosition() >= targetPosition)||(getCurrentPosition() == Settings->MaxPosition))
        {
            TurnSolar(solOff);
            targetPosition = 0;
            AutoPositioningOn = 0;
        }
        break;
    case solEast:
        if((getCurrentPosition() <= targetPosition)||(getCurrentPosition() == 0))
        {
            TurnSolar(solOff);
            targetPosition = 0;
            AutoPositioningOn = 0;
        }
        break;
    
    default:
        break;
    }

}
void ProjectClass::goToPosition(uint32 _value)
{
    if(!(Settings->Flags & flagInitialised))
        return;
    if(_value > Settings->MaxPosition)
        return;
    if(OutputSolarState != solOff)
        return;
    
    if(_value > getCurrentPosition())
    {
        targetPosition = _value;
        TurnSolar(solWest);
        AutoPositioningOn = 1;
        return;
    }
    if(_value < getCurrentPosition())
    {
        targetPosition = _value;
        TurnSolar(solEast);
        AutoPositioningOn = 1;
        return;
    }
}
void ProjectClass::goToStart()
{
    goToPosition(getStartPosition());
}
void ProjectClass::goToEnd()
{
    goToPosition(getEndPosition());
}
void ProjectClass::goToTime(uint8 Direction, uint16 Time_ms)
{
    if(getAutoStateFlag())
        return;
    if(isError())
        return;
    if(OutputSolarState != solOff)
        return;
    if(!((Direction == solEast)||(Direction == solWest)))
        return;
    TimeToStop = millis() + Time_ms;
    TurnSolar(Direction);  
}
void ProjectClass::startAutoMode()
{
    Settings->Flags = Settings->Flags | flagAutoModeOn;
}
void ProjectClass::stopAutoMode()
{
    Settings->Flags = Settings->Flags & ~((uint16) flagAutoModeOn);
}
uint8 ProjectClass::getAutoStateFlag()
{
    return ((Settings->Flags & flagAutoModeOn)/flagAutoModeOn);
}
uint8 ProjectClass::getIsNotInit()
{
    return ((Settings->Flags & flagInitialised)?0:1);
}
ProjectConfig * ProjectClass::getSettings()
{
    return Settings;
}

void ProjectClass::setTimeStart(String _Clock)
{
    uint8 Temp[2];
    uint16 tempMinutes, tempStop, tempTurnBack;
    tempStop = getMinutes(Settings->TimeEnd);
    tempTurnBack = getMinutes(Settings->TimeTurnBack);
    if(sscanf(_Clock.c_str(), "%hhu:%hhu", &Temp[tmHour], &Temp[tmMinute])!=2)
        return;
    if(!checkTime(Temp))
        return;
    tempMinutes = getMinutes(Temp);

    if(tempMinutes >= tempStop)
        return;
    if((tempTurnBack <= tempStop)&&(tempTurnBack >= tempMinutes))
        return;

    Settings->TimeStart[tmHour] = Temp[tmHour];
    Settings->TimeStart[tmMinute] = Temp[tmMinute];
}
void ProjectClass::setTimeStop(String _Clock)
{
    uint8 Temp[2];
    uint16 tempMinutes, tempStart, tempTurnBack;
    tempStart = getMinutes(Settings->TimeStart);
    tempTurnBack = getMinutes(Settings->TimeTurnBack);
    if(sscanf(_Clock.c_str(), "%hhu:%hhu", &Temp[tmHour], &Temp[tmMinute])!=2)
        return;
    if(!checkTime(Temp))
        return;
    tempMinutes = getMinutes(Temp);

    if(tempMinutes <= tempStart)
        return;
    if((tempTurnBack >= tempStart)&&(tempTurnBack <= tempMinutes))
        return;

    Settings->TimeEnd[tmHour] = Temp[tmHour];
    Settings->TimeEnd[tmMinute] = Temp[tmMinute];
}
void ProjectClass::setTimeTurnBack(String _Clock)
{
    uint8 Temp[2];
    uint16 tempMinutes, tempStart, tempStop;
    tempStart = getMinutes(Settings->TimeStart);
    tempStop = getMinutes(Settings->TimeEnd);
    if(sscanf(_Clock.c_str(), "%hhu:%hhu", &Temp[tmHour], &Temp[tmMinute])!=2)
        return;
    if(!checkTime(Temp))
        return;
    tempMinutes = getMinutes(Temp);

    if((tempMinutes >= tempStart)&&(tempMinutes <= tempStop))
        return;

    Settings->TimeTurnBack[tmHour] = Temp[tmHour];
    Settings->TimeTurnBack[tmMinute] = Temp[tmMinute];
}
void ProjectClass::setTime(String _Start, String _End, String _TurnBack)
{
    uint8 tmpStart[2], tmpStop[2], tmpTurnBack[2];
    uint16 tempMinTurnBack, tempMinStart, tempMinStop;

    if(sscanf(_Start.c_str(), "%hhu:%hhu", &tmpStart[tmHour], &tmpStart[tmMinute])!=2)
        return;
    if(sscanf(_End.c_str(), "%hhu:%hhu", &tmpStop[tmHour], &tmpStop[tmMinute])!=2)
        return;
    if(sscanf(_TurnBack.c_str(), "%hhu:%hhu", &tmpTurnBack[tmHour], &tmpTurnBack[tmMinute])!=2)
        return;
    if(!checkTime(tmpStart)||!checkTime(tmpStop)||!checkTime(tmpTurnBack))
        return;

    tempMinStart = getMinutes(tmpStart);
    tempMinStop = getMinutes(tmpStop);
    tempMinTurnBack = getMinutes(tmpTurnBack);

    if(tempMinStop <= tempMinStart)
        return;
    if((tempMinTurnBack >= tempMinStart)&&(tempMinTurnBack <= tempMinStop))
        return;

    Settings->TimeStart[tmHour] = tmpStart[tmHour];
    Settings->TimeStart[tmMinute] = tmpStart[tmMinute];
    Settings->TimeEnd[tmHour] = tmpStop[tmHour];
    Settings->TimeEnd[tmMinute] = tmpStop[tmMinute];
    Settings->TimeTurnBack[tmHour] = tmpTurnBack[tmHour];
    Settings->TimeTurnBack[tmMinute] = tmpTurnBack[tmMinute];
}
void ProjectClass::setTimeAutoBreak(uint16 _Minutes)
{
    Settings->BreakMinute = _Minutes;
}
uint16 ProjectClass::getTimeAutoBreak()
{
    return Settings->BreakMinute;
}
String ProjectClass::getTimeStart()
{
    char Temp[10];
    sprintf(Temp, "%02hhu:%02hhu", Settings->TimeStart[tmHour], Settings->TimeStart[tmMinute]);
    return Temp;
}
String ProjectClass::getTimeEnd()
{
    char Temp[10];
    sprintf(Temp, "%02hhu:%02hhu", Settings->TimeEnd[tmHour], Settings->TimeEnd[tmMinute]);
    return Temp;
}
String ProjectClass::getTimeTurnBack()
{
    char Temp[10];
    sprintf(Temp, "%02hhu:%02hhu", Settings->TimeTurnBack[tmHour], Settings->TimeTurnBack[tmMinute]);
    return Temp;
}

void ProjectClass::incrementCounter()
{
    Settings->CurrentPosition++;
    anyPosChange=1;
}
void ProjectClass::decrementCounter()
{
    if(Settings->CurrentPosition)
    {
        Settings->CurrentPosition--;
        anyPosChange=1;
    }
}
void ProjectClass::incrementCounterFailure()
{
    counterFailure++;
    Settings->Flags &= ~((uint16) flagInitialised | (uint16) flagAutoModeOn);
}
uint16 ProjectClass::resetCounterFailure()
{
    uint16 Temp = counterFailure;
    counterFailure = 0;
    return Temp;
}
uint16 ProjectClass::getCouterFailure()
{
    return counterFailure;
}
void ProjectClass::setMaxPosition(uint32 _Value)
{
    if(_Value < 20)
        return;
    
    Settings->MaxPosition = _Value;
    if(Settings->EndPosition > _Value)
        Settings->EndPosition = _Value;
}
uint32 ProjectClass::getMaxPosition()
{
    return Settings->MaxPosition;
}
/*void ProjectClass::setCurrentPosition(uint32 _Value)
{
    if(Settings->MaxPosition < _Value)
        return;
    Settings->CurrentPosition = _Value;
}*/
uint32 ProjectClass::getCurrentPosition()
{
    return Settings->CurrentPosition;
}
void ProjectClass::setStartPosition(uint32 _Value)
{
    if((Settings->MaxPosition <= _Value)||(Settings->EndPosition <= _Value))
        return;
    Settings->StartPosition = _Value;
}
uint32 ProjectClass::getStartPosition()
{
    return Settings->StartPosition;
}
void ProjectClass::setEndPosition(uint32 _Value)
{
    if((Settings->MaxPosition <= _Value)||(Settings->StartPosition >= _Value))
        return;
    Settings->EndPosition = _Value;

}
uint32 ProjectClass::getEndPosition()
{
    return Settings->EndPosition;
}
uint8 ProjectClass::getOutputSolarState()
{
    if((OutputSolarState == solOff) && (millis()<(OutputSolarLastChange + OutputSolarChangeLock)))
        return OutputSolarStateBefore + 2;
    return OutputSolarState;
}
bool ProjectClass::anyChange()
{
    bool Temp = (anyPosChange==1);
    anyPosChange = 0;
    return Temp;
}
void ProjectClass::StartReference()
{
    referenceState = 1;
}
void ProjectClass::AbortReference()
{
    referenceState = 0;
}
uint8 ProjectClass::getReferenceState()
{
    return referenceState;
}
void ProjectClass::resetErrorFlag()
{
    Settings->Flags &=  ~((uint16) flagError);
}
uint8 ProjectClass::isError()
{
    return (Settings->Flags & flagError) / flagError;
}
void ProjectClass::ReferenceLoop()
{
    switch (referenceState)
    {
    case 1:
        Settings->Flags &= ~((uint16) flagInitialised);
        TurnSolar(solWest);
        LastPosChange = millis();
        referenceState++;
        break;
    case 2:
        if((LastPosChange + 10000) < millis())
        {
            Settings->CurrentPosition = -1;
            TurnSolar(solOff);
            LastPosChange = millis();
            referenceState++;
        }
        break;
    case 3:
        if((LastPosChange + OutputSolarChangeLock) < millis())
        {
            TurnSolar(solEast);
            LastPosChange = millis();
            referenceState++;
        }
        break;
    case 4:
        if((LastPosChange + 10000) < millis())
        {
            TurnSolar(solOff);
            Settings->MaxPosition = 0 - Settings->CurrentPosition + 1;
            if(Settings->MaxPosition < 20)
            {
                referenceState = 0;
                break;
            }
            Settings->CurrentPosition = 0;
            Settings->EndPosition = Settings->MaxPosition -1;
            Settings->StartPosition = 1;
            Settings->Flags |= ((uint16) flagInitialised);
            referenceState = 0;
        }
        break;
    
    default:
        break;
    }
}
uint16 ProjectClass::getMinutes(uint8 * _Time)
{
    return (_Time[tmMinute]+(_Time[tmHour]*60));
}
uint16 ProjectClass::getMinutes(String _Time)
{
    uint8 Temp[2];
    if(sscanf(_Time.c_str(), "%hhu:%hhu", &Temp[tmHour], &Temp[tmMinute])!=2)
        return 0;
    if((Temp[tmHour] >= 24)||(Temp[tmMinute] >= 60))
        return 0;
    return (getMinutes(Temp));
}
bool ProjectClass::checkTime(uint8 * _Time)
{
    if(_Time[tmHour] >= 24)
        return 0;
    if(_Time[tmMinute] >= 60)
        return 0;
    
    return 1;
}