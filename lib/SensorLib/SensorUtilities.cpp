#include "SensorUtilities.h"


namespace SensorProject
{
    
    bool parsePrintSummarySig(std::map<String,String> & keyValPairs)
    {
        std::map<String, String>::iterator iterStart = keyValPairs.begin(), iterEnd = keyValPairs.end();

        if(iterStart == iterEnd || keyValPairs.size() < SENSOR_PRINTSUMMARY_PAIRS || keyValPairs.size() > SENSOR_PRINTSUMMARY_PAIRS) return false;

        while(iterStart != iterEnd)
        {   
            String key = iterStart->first;
            String value = iterStart->second;

            if(key == WEB_KEY_TASK && value == WEB_VAL_SENSOR)
            {
                return true;
            }
            
            iterStart++;
        }
        
        return false;
    }

    bool parseWebdata(eSigErr & signal,std::map<String,String> & keyValPairs)
    {
        std::map<String, String>::iterator iterStart = keyValPairs.begin(), iterEnd = keyValPairs.end();

        if(iterStart == iterEnd || (keyValPairs.size() < SENSOR_MAX_COMMANDPAIRS ||  keyValPairs.size() > SENSOR_MAX_COMMANDPAIRS)){
            return false; 
        }   

        while(iterStart != iterEnd)
        {   
            String key = iterStart->first;
            String value = iterStart->second;

            if(key.equals(WEB_KEY_ACTION))
            {
                if(value.equals(WEB_VALUE_MEASURELS))
                {
                    signal = eSigErr::eMEASURELIGHTSENSOR;
                }
                else if(value.equals(WEB_VALUE_MEASUREUS))
                {
                    signal = eSigErr::eMEASUREULTRASONIC;
                }
                else if(value.equals(WEB_VALUE_CLEARLS))
                {
                    signal = eSigErr::eCLEARLIGHTSENSOR;
                }
                else if(value.equals(WEB_VALUE_CLEARUS))
                {
                    signal = eSigErr::eCLEARULTRASONIC;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }

            iterStart++;
        }
        return true;
    }


    bool measureSensorData(eSigErr signal,SensorData & sensordata)
    {
        if(signal == eSigErr::eMEASURELIGHTSENSOR)
        {
            if(sensordata.u8LightSensor == SENSOR_MAX_DATABUFFER) return false;
            /* https://esp32io.com/tutorials/esp32-light-sensor */
            sensordata.lightSensor[sensordata.u8LightSensor++] = analogRead(SENSOR_LIGHT_PIN);
        }
        else if(signal == eSigErr::eMEASUREULTRASONIC)
        {
            if(sensordata.u8BuffLenUS == SENSOR_MAX_DATABUFFER) return false;
            
            /* https://randomnerdtutorials.com/esp32-hc-sr04-ultrasonic-arduino/ */
            digitalWrite(SENSOR_ULTRASO_PIN2,LOW);
            delay(2);
            digitalWrite(SENSOR_ULTRASO_PIN2,HIGH);
            delay(10);
            digitalWrite(SENSOR_ULTRASO_PIN2,LOW);

            long duration = pulseIn(SENSOR_ULTRASO_PIN1,HIGH);
            sensordata.ultrasonicSensordata[sensordata.u8BuffLenUS++] = duration * SENSOR_SOUND_SPEED / 2;
        }
        return true;
    }
}