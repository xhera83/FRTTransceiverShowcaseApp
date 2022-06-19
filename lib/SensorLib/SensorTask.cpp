#include "Arduino.h"
#include "FRTTransceiver.h"
#include <map>
#include "Global_FRTTSetup.h"

#include "SensorTask.h"
#include "SensorUtilities.h"


void SensorTask(void * pvParams)
{   
    while(WEBSERVER_TASKHANDLE == NULL) vTaskDelay(pdMS_TO_TICKS(1));
    vTaskDelay(pdMS_TO_TICKS(500));
    
    FRTT::FRTTransceiver comm(SENSOR_MAIN_TASKHANDLE,1);

    pinMode(SENSOR_ULTRASO_PIN2,OUTPUT);
    pinMode(SENSOR_ULTRASO_PIN1,INPUT);


    comm.addDataAllocateCallback(dataAllocator);
    comm.addDataFreeCallback(deleter);

    comm.addCommPartner(WEBSERVER_TASKHANDLE,SENSOR_RX_QUEUE,1,SENSOR_RX_SEM,SENSOR_TX_QUEUE,1,SENSOR_TX_SEM,"WEBSERVER");

    SensorProject::SensorData sensordata;
    SensorProject::eSigErr signals;

    for(;;)
    {   
        if(comm.readFromQueue(WEBSERVER_TASKHANDLE,FRTT::eMultiSenderQueue::eNOMULTIQSELECTED,true,50,50)){
            const FRTT::FRTTTempDataContainer * t = comm.getNewestBufferedDataFrom(WEBSERVER_TASKHANDLE,FRTT::eMultiSenderQueue::eNOMULTIQSELECTED,true);

            if(t != NULL)
            {
                /* move over */
                std::map<String,String> keyValPairs = *((std::map<String,String>  *)t->data);

                if(SensorProject::parseWebdata(signals,keyValPairs))
                {
                    switch(signals)
                    {
                        case SensorProject::eSigErr::eCLEARLIGHTSENSOR:
                            /* clear sensor data */
                            sensordata.u8LightSensor = 0;
                            break;
                        case SensorProject::eSigErr::eCLEARULTRASONIC:
                            /* clear sensor data */
                            sensordata.u8BuffLenUS = 0;
                            break;
                        case SensorProject::eSigErr::eMEASURELIGHTSENSOR:
                            /* measure new values humidity temperature.. */
                        case SensorProject::eSigErr::eMEASUREULTRASONIC:
                            /* measure new values ultrasonic...*/
                            if(!SensorProject::measureSensorData(signals,sensordata))
                            {
                                comm.writeToQueue(WEBSERVER_TASKHANDLE,(uint8_t)Datatypes::eSENSORDATA,&sensordata,FRTTRANSCEIVER_WAITMAX,FRTTRANSCEIVER_WAITMAX,(uint32_t)SensorProject::eSigErr::eERROR);
                                continue;
                            }
                            break;
                    }

                    if(signals == SensorProject::eSigErr::eCLEARLIGHTSENSOR || signals == SensorProject::eSigErr::eCLEARULTRASONIC)
                    {
                        signals = SensorProject::eSigErr::eCLEARSUCCEEDED;
                    }
                    else if(signals == SensorProject::eSigErr::eMEASURELIGHTSENSOR || signals == SensorProject::eSigErr::eMEASUREULTRASONIC)
                    {
                        signals = SensorProject::eSigErr::eMEASURESUCCEEDED;
                    }

                    comm.writeToQueue(WEBSERVER_TASKHANDLE,(uint8_t)Datatypes::eSENSORDATA,&sensordata,FRTTRANSCEIVER_WAITMAX,FRTTRANSCEIVER_WAITMAX,(uint32_t)signals);
                    continue;
                }
                else
                {
                    comm.writeToQueue(WEBSERVER_TASKHANDLE,(uint8_t)Datatypes::eSENSORDATA,&sensordata,FRTTRANSCEIVER_WAITMAX,FRTTRANSCEIVER_WAITMAX,(uint32_t)SensorProject::eSigErr::eERROR);
                    continue;
                }
            }
        }
        else if(comm.NotifyReceiveBasic(FRTT::eFRTTNotifyActions::e_CLEARCOUNTONEXIT,50).getNotificationVal() != 0)
        {
            comm.printCommunicationsSummary();
        }
        
    }

    vTaskDelete(NULL);
} 