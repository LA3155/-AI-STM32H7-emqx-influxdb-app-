#include "init.h"
#include "Lora.h"
#include "MQTT.h"
#include "Edge_ai_app.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

osMessageQueueId_t sizequeue;

void initTask(void *argument)
{
    LoRa_Init();
    Edge_AI_Init();
    sizequeue = osMessageQueueNew(1, sizeof(uint16_t),NULL);
    vTaskDelete(NULL);
}