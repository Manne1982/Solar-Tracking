#include "Project_Structures.h"
#include "GlobalVariabels.h"


ProjectClass::ProjectClass():
counterFailure(0),
OutputSolarLastChange(0),
OutputSolarChangeLock(5000),
OutputSolarState(0),
targetPosition(0),
AutoPositioningOn(0)
{
    Settings = new ProjectConfig;
    Settings->TimeEnd[tmHour] = 22;
    Settings->TimeEnd[tmMinute] = 59;
    Settings->TimeStart[tmHour] = 5;
    Settings->TimeStart[tmMinute] = 0;
    Settings->TimeTurnBack[tmHour] = 4;
    Settings->TimeTurnBack[tmHour] = 0;
}

ProjectClass::~ProjectClass()
{
    delete Settings;
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

  pinMode(CounterPinPosition, INPUT); 
}

void ProjectClass::TurnSolar(uint8 _value)
{
  switch (_value)
  {
  case solOff:
    digitalWrite(OutputTurnOnOff, 1);
    digitalWrite(OutputTurnDirection, 1);
    OutputSolarLastChange = millis();
    OutputSolarState = solOff;
    break;
  case solWest:
    if(((OutputSolarLastChange + OutputSolarChangeLock)>millis())||(OutputSolarState == solEast))
      break;
    digitalWrite(OutputTurnDirection, 1);
    digitalWrite(OutputTurnOnOff, 0);
    OutputSolarLastChange = millis();
    OutputSolarState = solWest;
    break;
  case solEast:
    if(((OutputSolarLastChange + OutputSolarChangeLock)>millis())||(OutputSolarState == solWest))
      break;
    digitalWrite(OutputTurnDirection, 0);
    digitalWrite(OutputTurnOnOff, 0);
    OutputSolarLastChange = millis();
    OutputSolarState = solEast;
    break;
  
  default:
    break;
  }
}
void ProjectClass::loop()
{
    switch (OutputSolarState)
    {
    case solWest:
        if(getCurrentPosition() >= targetPosition)
        {
            TurnSolar(solOff);
            targetPosition = 0;
            AutoPositioningOn = 0;
        }
        break;
    case solEast:
        if(getCurrentPosition() <= targetPosition)
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
    if((tempMinTurnBack >= tempMinStart)&&(tempMinTurnBack <= tempMinStop))
        return;

    Settings->TimeStart[tmHour] = tmpStart[tmHour];
    Settings->TimeStart[tmMinute] = tmpStart[tmMinute];
    Settings->TimeEnd[tmHour] = tmpStop[tmHour];
    Settings->TimeEnd[tmMinute] = tmpStop[tmMinute];
    Settings->TimeTurnBack[tmHour] = tmpTurnBack[tmHour];
    Settings->TimeTurnBack[tmMinute] = tmpTurnBack[tmMinute];
}
void ProjectClass::incrementCounter()
{
    Settings->CurrentPosition++;
}
void ProjectClass::decrementCounter()
{
    if(Settings->CurrentPosition)
        Settings->CurrentPosition--;
}
void ProjectClass::incrementCounterFailure()
{
    counterFailure++;
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
void ProjectClass::setCurrentPosition(uint32 _Value)
{
    if(Settings->MaxPosition < _Value)
        return;
    Settings->CurrentPosition = _Value;
}
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
    return OutputSolarState;
}


uint16 ProjectClass::getMinutes(uint8 * _Time)
{
    return (_Time[tmMinute]+(_Time[tmHour]*60));
}
bool ProjectClass::checkTime(uint8 * _Time)
{
    if(_Time[tmHour] >= 24)
        return 0;
    if(_Time[tmMinute] >= 60)
        return 0;
    
    return 1;
}