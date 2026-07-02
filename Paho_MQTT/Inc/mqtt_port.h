#ifndef __MQTT_PORT_H
#define __MQTT_PORT_H

#include "main.h"
#include "cmsis_os2.h" // FreeRTOS 的头文件

// 1. 定义时间管理结构体 (使用 FreeRTOS 的 Tick)
typedef struct Timer {
    uint32_t end_time; // 记录超时的绝对时间点
} Timer;

// 2. 定义网络接口结构体
typedef struct Network {
    int (*mqttread)(struct Network*, unsigned char* read_buffer, int len, int timeout_ms);
    int (*mqttwrite)(struct Network*, unsigned char* send_buffer, int len, int timeout_ms);
} Network;

// 提供一个初始化 Network 的函数原型
void NetworkInit(Network* n);
void RingBuf_Write(uint8_t *data, uint16_t len);
int  RingBuf_Read(uint8_t *data, uint16_t len);
void RingBuf_Clear(void);

#endif /* __MQTT_PORT_H */