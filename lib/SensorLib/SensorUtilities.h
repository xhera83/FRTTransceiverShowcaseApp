#include <cstdint>
#include <map>
#include <WString.h>
#include <Arduino.h>

#include "Global_WebPage.h"

#define SENSOR_MAX_DATABUFFER          (10U)
#define SENSOR_MAX_COMMANDPAIRS        (1U)
#define SENSOR_PRINTSUMMARY_PAIRS   (1U)            /* key-val pairs in url */ 

#define SENSOR_ULTRASO_PIN1            (18U)
#define SENSOR_ULTRASO_PIN2            (5U)
#define SENSOR_SOUND_SPEED             (0.034)

#define SENSOR_LIGHT_PIN               (36U)    /* 10kOhm widerstand noch an die Schaltung */


namespace SensorProject 
{
    struct SensorData 
    {
        float ultrasonicSensordata[SENSOR_MAX_DATABUFFER];
        uint8_t u8BuffLenUS = 0;

        int lightSensor[SENSOR_MAX_DATABUFFER];
        uint8_t u8LightSensor = 0;
    };

    enum class eSigErr : uint8_t{
        eNULL = 0,
        eMEASUREULTRASONIC,
        eMEASURELIGHTSENSOR,
        eCLEARULTRASONIC,
        eCLEARLIGHTSENSOR,
        eCLEARSUCCEEDED,
        eMEASURESUCCEEDED,
        eERROR
    };
    
    bool parsePrintSummarySig(std::map<String,String> &); 
    bool parseWebdata(eSigErr &,std::map<String,String> &);
    bool measureSensorData(eSigErr signal,SensorData & sensordata);

}