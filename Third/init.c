#include "init.h"
#include "Lora.h"
#include "MQTT.h"
#include "Edge_ai_app.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "task.h"
#include "cmsis_os.h"
#include <string.h>

osMessageQueueId_t sizequeue;
osMessageQueueId_t loraqueue;
extern struct lora_t lora;

void initTask(void *argument)
{
    LoRa_Init();
    Edge_AI_Init();
    sizequeue = osMessageQueueNew(1, sizeof(uint16_t),NULL);
    loraqueue = osMessageQueueNew(10,sizeof(lora_t),NULL);
    vTaskDelete(NULL);
}