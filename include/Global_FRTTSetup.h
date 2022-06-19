#ifndef GLOBAL_TASKS_H
#define GLOBA_TASKS_H

#include "FRTTransceiver.h"

#define HIGHEST_TASK_PRIORITY (8u)

enum class Datatypes : uint8_t
{
    eKEYVAL = 0,
    eSENSORDATA,
    eDUMMYDATA
};

/* WEBSERVER - TASK */
extern FRTT::FRTTTaskHandle WEBSERVER_TASKHANDLE;
/* SMARTDOOR - TASK*/
extern FRTT::FRTTTaskHandle SMARTDOOR_MAIN_TASKHANDLE;

extern FRTT::FRTTQueueHandle SMARTDOOR_TX_QUEUE;
extern FRTT::FRTTQueueHandle SMARTDOOR_RX_QUEUE;

extern FRTT::FRTTSemaphoreHandle SMARTDOOR_TX_SEM;
extern FRTT::FRTTSemaphoreHandle SMARTDOOR_RX_SEM;

/* SENSOR - TASK*/
extern FRTT::FRTTTaskHandle SENSOR_MAIN_TASKHANDLE;

extern FRTT::FRTTQueueHandle SENSOR_TX_QUEUE;
extern FRTT::FRTTQueueHandle SENSOR_RX_QUEUE;

extern FRTT::FRTTSemaphoreHandle SENSOR_TX_SEM;
extern FRTT::FRTTSemaphoreHandle SENSOR_RX_SEM;

/* Callbacks */
void dataAllocator(const FRTT::FRTTDataContainerOnQueue & orig, FRTT::FRTTTempDataContainer & temp);
void deleter(FRTT::FRTTTempDataContainer & temp);

#endif