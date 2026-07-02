#ifndef __MQTT_H
#define __MQTT_H

#include "main.h"
#include "cmsis_os2.h" // 包含 FreeRTOS v2 的 API

/* 宏定义，方便后续修改服务器参数 */
#define EMQX_CLIENT_ID  "stm32H7"

#define EMQX_SERVER_IP  "129.204.158.52"  // 云服务器公网 IP
#define EMQX_SERVER_PORT "1883"           // 标准 MQTT TCP 端口

// #define EMQX_SERVER_IP  "frp-put.com"  // 请替换为你的 SakuraFrp 域名
// #define EMQX_SERVER_PORT "64070"                    // 请替换为你的端口号:

/* 对外暴露的函数原型 */
uint8_t Send_AT_Command_RTOS(char *cmd, char *ack, uint32_t timeout_ms);
void mqttTask(void *argument);
uint8_t crc8_calc_shift(uint8_t *data, uint16_t len);

#endif /* __MQTT_H */