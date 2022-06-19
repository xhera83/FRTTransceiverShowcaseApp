#include <Arduino.h>
#include "ShowcaseWebserver.h"
#include "SensorTask.h"
#include "SmartDoorTask.h"
#include "Global_FRTTSetup.h"

#include "UnitTest.h"


FRTT::FRTTTaskHandle WEBSERVER_TASKHANDLE;
FRTT::FRTTTaskHandle SMARTDOOR_MAIN_TASKHANDLE;
FRTT::FRTTQueueHandle SMARTDOOR_TX_QUEUE;
FRTT::FRTTQueueHandle SMARTDOOR_RX_QUEUE;

FRTT::FRTTSemaphoreHandle SMARTDOOR_TX_SEM;
FRTT::FRTTSemaphoreHandle SMARTDOOR_RX_SEM;

FRTT::FRTTTaskHandle SENSOR_MAIN_TASKHANDLE;

FRTT::FRTTQueueHandle SENSOR_TX_QUEUE;
FRTT::FRTTQueueHandle SENSOR_RX_QUEUE;

FRTT::FRTTSemaphoreHandle SENSOR_TX_SEM;
FRTT::FRTTSemaphoreHandle SENSOR_RX_SEM;



void setup() {
	#if (UNITTEST == 0)
	SMARTDOOR_TX_QUEUE = FRTT::FRTTCreateQueue(1);
	SMARTDOOR_RX_QUEUE = FRTT::FRTTCreateQueue(1);

	SMARTDOOR_TX_SEM = FRTT::FRTTCreateSemaphore();
	SMARTDOOR_RX_SEM = FRTT::FRTTCreateSemaphore();

	SENSOR_TX_QUEUE = FRTT::FRTTCreateQueue(1);
	SENSOR_RX_QUEUE = FRTT::FRTTCreateQueue(1);

	SENSOR_TX_SEM = FRTT::FRTTCreateSemaphore();
	SENSOR_RX_SEM = FRTT::FRTTCreateSemaphore();


	FRTT::FRTTCreateTask(WebserverTask,"webserver",5000,NULL,HIGHEST_TASK_PRIORITY,&WEBSERVER_TASKHANDLE,0);
	FRTT::FRTTCreateTask(SmartDoorTask,"smartdoor",3000,NULL,HIGHEST_TASK_PRIORITY - 1,&SMARTDOOR_MAIN_TASKHANDLE,1);
	FRTT::FRTTCreateTask(SensorTask,"sensor",3000,NULL,HIGHEST_TASK_PRIORITY - 1,&SENSOR_MAIN_TASKHANDLE,1);
	#else
	vTaskDelay(pdMS_TO_TICKS(2500));
    Serial.begin(115200);               // For the internal printer of TestRunner
    disableCore0WDT();
    
	while(!Serial);                     
    printf("\n\n");
    

    printf("This test should produce the following:\n");
    printf("4 passed, 0 failed, 0 skipped, 0 timed out, out of 4 test(s).\n");
    printf("----------\n\n");
	#endif
}

void loop() {
	#if (UNITTEST == 0)
	// put your main code here, to run repeatedly:
	delay(10000);
	#else
	TestRunner::run();
	#endif
}