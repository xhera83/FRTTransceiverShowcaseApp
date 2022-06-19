#include "Arduino.h"
#include "FRTTransceiver.h"
#include <map>

#include "Global_FRTTSetup.h"
#include "SmartDoorUtilities.h"
#include "SmartDoorTask.h"


void SmartDoorTask(void *)
{   
    vTaskDelay(pdMS_TO_TICKS(500));
    
    FRTT::FRTTransceiver comm(SMARTDOOR_MAIN_TASKHANDLE,1);
    
    SmartDoorProject::pwm.allocateTimer(0);

    SmartDoorProject::servo.setPeriodHertz(50);
    SmartDoorProject::servo.attach(SMARTDOOR_LOCK_PIN);
    SmartDoorProject::servo.write(0);
    
    SmartDoorProject::strip.begin();
    SmartDoorProject::strip.clear();
    SmartDoorProject::strip.show();
    
    comm.addDataAllocateCallback(dataAllocator);
    comm.addDataFreeCallback(deleter);

    comm.addCommPartner(WEBSERVER_TASKHANDLE,SMARTDOOR_RX_QUEUE,1,SMARTDOOR_RX_SEM,SMARTDOOR_TX_QUEUE,1,SMARTDOOR_TX_SEM,"WEBSERVER");

    int dummy = 0;

    SDRControl ringControl;
    SDMControl mailboxControl;
    
    SmartDoorProject::eSigErr signals = SmartDoorProject::eSigErr::eNULL;

    for(;;)
    {
        if(comm.readFromQueue(WEBSERVER_TASKHANDLE,FRTT::eMultiSenderQueue::eNOMULTIQSELECTED,true,50,50))
        {
            const FRTT::FRTTTempDataContainer * t =  comm.getNewestBufferedDataFrom(WEBSERVER_TASKHANDLE,FRTT::eMultiSenderQueue::eNOMULTIQSELECTED,true);

            if(t != NULL)
            {   
                /* move over */
                std::map<String,String> keyValPairs = *((std::map<String,String>  *)t->data);
                signals = (SmartDoorProject::eSigErr)t->u32AdditionalData;

                switch(signals)
                {
                    case SmartDoorProject::eSigErr::eRING:
                        /* USER WANTED TO RING THE DOOR */
                        if(SmartDoorProject::parseWebdata(ringControl,keyValPairs))
                        {   
                            if(SmartDoorProject::SMARTDOOR_LEDSTRIP_TASKHANDLE != nullptr)
                            {
                                vTaskDelete(SmartDoorProject::SMARTDOOR_LEDSTRIP_TASKHANDLE);
                                SmartDoorProject::strip.clear();
                                SmartDoorProject::strip.show();
                            }

                            ringControl.u8CurrRingTime = 0;

                            xTaskCreatePinnedToCore(SmartDoorProject::LedstripTask,"ledstrip-task",
                                            9000,&ringControl,HIGHEST_TASK_PRIORITY,&SmartDoorProject::SMARTDOOR_LEDSTRIP_TASKHANDLE,1);
                            
                            signals = (ringControl.currRingType == SmartDoorProject::eRingType::eBLINK) ? (SmartDoorProject::eSigErr::eBLINKSUCCEEDED):(SmartDoorProject::eSigErr::eWAVESUCCEEDED);
                        }
                        else
                        {
                            signals = SmartDoorProject::eSigErr::eERROR;
                        }
                        break;
                    case SmartDoorProject::eSigErr::eMAILBOX:
                        /* USER WANTED TO OPEN/CLOSE THE MAILBOX */
                        if(SmartDoorProject::parseWebdata(mailboxControl,keyValPairs))
                        {   
                            if(mailboxControl.status == SmartDoorProject::eMailbox::eCLOSED)
                            {  
                                signals = SmartDoorProject::eSigErr::eCLOSESUCCEEDED;
                            }
                            else
                            {
                                signals = SmartDoorProject::eSigErr::eOPENSUCCEEDED;
                            }

                            xTaskCreatePinnedToCore(SmartDoorProject::ServoLockTask,"servo-lock",1000,&mailboxControl,HIGHEST_TASK_PRIORITY - 1,NULL,0);
                        }
                        else
                        {
                            signals = SmartDoorProject::eSigErr::eERROR;
                        }
                        break;
                    case SmartDoorProject::eSigErr::eNULL:
                    default:
                        /* NULL signal or unknown signal */
                        signals = SmartDoorProject::eSigErr::eERROR;
                        break;
                }
    
                comm.delAllDatabuffForLine(WEBSERVER_TASKHANDLE,FRTT::eMultiSenderQueue::eNOMULTIQSELECTED,true);
            }
            comm.writeToQueue(WEBSERVER_TASKHANDLE,(uint8_t)Datatypes::eDUMMYDATA,&dummy,10,10,(uint32_t)signals);
        }
        else if(comm.NotifyReceiveBasic(FRTT::eFRTTNotifyActions::e_CLEARCOUNTONEXIT,50).getNotificationVal() != 0)
        {
            comm.printCommunicationsSummary();
        }
    }

    vTaskDelete(NULL);
} 
