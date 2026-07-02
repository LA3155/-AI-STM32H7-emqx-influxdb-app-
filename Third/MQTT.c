#include "MQTT.h"
#include "MQTTClient.h" 
#include "cmsis_os.h"
#include "cmsis_os2.h"
#include "mqtt_port.h"  
#include "usart.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "Edge_ai_app.h"
#include "stdlib.h"
#include "init.h"

#define AIR780E_UART huart5

extern uint8_t *lora_rx_buf;

#define MQTT_BUF_SIZE 512
MQTTClient client;
unsigned char sendbuf[MQTT_BUF_SIZE];
unsigned char readbuf[MQTT_BUF_SIZE];
char at_cmd_buf[128];
char json_payload[256];

//初始化阈值
threshold_t ai_threshold =
{
    .high_temp = 0.02f,
    .storm     = 0.65f,
    .hyphoon   = 0.90f,
};

char payload_buf[80]={0};
char *colon;


float ai_input[4];
extern void RingBuf_Clear(void); 
extern int RingBuf_Read(uint8_t *data, uint16_t len);
extern float latest_ai_probabilities[4];
//控制主题
char topic_ctrol[]="iot/device";


//定义函数指针
typedef void (*messageHandler_subscribe_t)(MessageData*);
typedef void (*mqttdata_t)(void *);
extern globaldata_t globaldata;
extern osEventFlagsId_t mqtt_event_flags;

void MQTT_Time_CallBack()
{
    sprintf(json_payload, "{\"temp\": %.2f, \"humi\": %.2f, \"light\": %.2f, \"press\": %.2f, \"wind\": %.2f,\"status\": %d, \"p1\": %d, \"p2\": %d, \"p3\": %d}",
    globaldata.sensor_data[0],globaldata.sensor_data[1], globaldata.sensor_data[2], globaldata.sensor_data[4],globaldata.sensor_data[3], \
    globaldata.alarm_status, (int)(globaldata.probabilities[1]*100),(int)(globaldata.probabilities[2]*100),(int)(globaldata.probabilities[3]*100));

    printf("%s",json_payload);
    MQTTMessage message;
    message.qos = QOS0;             
    message.retained = 0;           
    message.dup = 0;
    message.id = 0;
    message.payload = (void*)json_payload;
    message.payloadlen = strlen(json_payload); 
    //测试阶段不发数据
    // if (MQTTPublish(&client, "iot/environment", &message) == SUCCESS)
    // {
    //     printf("[MQTT Pub] 成功: %s\r\n", json_payload);
    // }
    // else
    // {
    //     printf("[MQTT Pub] 发送失败！\r\n");
    // }
}

uint8_t Send_AT_Command_RTOS(char *cmd, char *ack, uint32_t timeout_ms)
{
    char rx_buf[512] = {0}; 
    uint16_t rx_len = 0;
    uint32_t start_time = osKernelGetTickCount();

    HAL_UART_Transmit(&AIR780E_UART, (uint8_t *)cmd, strlen(cmd), 1000);
    printf(">> %s", cmd);

    while ((osKernelGetTickCount() - start_time) < timeout_ms)
    {
        uint8_t ch;
        if (RingBuf_Read(&ch, 1) == 1)
        {
            if (rx_len < sizeof(rx_buf) - 1)
            {
                rx_buf[rx_len++] = ch;
                rx_buf[rx_len] = '\0';
            }

            if (strstr(rx_buf, ack) != NULL)
            {
                printf("<< %s\r\n", rx_buf);
                return 1;
            }
        }
        else
        {
            osDelay(1);
        }
    }
    printf("<< 超时未收到预期回复 UART5! 已收: %s\r\n", rx_buf);
    return 0;
}

//SUBSCRIBE回调函数
void messageHandler_subscribe(MessageData* md)
{
    memcpy(payload_buf,md->message->payload,strlen(md->message->payload)+1);
    if(strstr(payload_buf,"\"upload\"") != NULL)
    {
        char *str = strstr(payload_buf,"\"p1\"");
        colon = strchr(str, ':');
        ai_threshold.high_temp = atof(colon + 1);
        str = strstr(payload_buf,"\"p2\"");
        colon = strchr(str, ':');
        ai_threshold.storm = atof(colon + 1);
        str = strstr(payload_buf,"\"p3\"");
        colon = strchr(str, ':');
        ai_threshold.hyphoon = atof(colon + 1);
    }
    else
    {
        return;
    }
    MQTT_Time_CallBack(lora_rx_buf);
}

void mqtt_tcpconnect(void)
{
    printf("\r\n=== 开始初始化 AIR780E 基础网络 ===\r\n");
    printf("--- 强行退出可能存在的透传状态 ---\r\n");
    HAL_UART_Transmit(&AIR780E_UART, (uint8_t *)"+++", 3, 1000);
    osDelay(1000); 
    RingBuf_Clear();

    while(!Send_AT_Command_RTOS("AT\r\n", "OK", 1000)) 
    { 
        HAL_UART_Transmit(&AIR780E_UART, (uint8_t *)"+++", 3, 1000);
        osDelay(1000);
    }

    //强制关闭模块本地回显！彻底根除 dma_rx_buf 中的 M Q T T 污染
    while(!Send_AT_Command_RTOS("ATE0\r\n", "OK", 1000)) { osDelay(1000); }
    
    while(!Send_AT_Command_RTOS("AT+CPIN?\r\n", "+CPIN: READY", 2000)) { osDelay(1000); }

    Send_AT_Command_RTOS("AT+CIPSHUT\r\n", "SHUT OK", 3000);
    
    while(!Send_AT_Command_RTOS("AT+CGATT=1\r\n", "OK", 5000)) { osDelay(1000); }

    Send_AT_Command_RTOS("AT+CIPMUX=0\r\n", "OK", 1500);
    Send_AT_Command_RTOS("AT+CIPMODE=1\r\n", "OK", 1500);

    sprintf(at_cmd_buf, "AT+CIPSTART=\"TCP\",\"%s\",%s\r\n", EMQX_SERVER_IP, EMQX_SERVER_PORT);
    
    while(!Send_AT_Command_RTOS(at_cmd_buf, "CONNECT\r\n", 8000))
    {
        printf("[TCP] 连接服务器失败，重试中...\r\n");
        Send_AT_Command_RTOS("AT+CIPSHUT\r\n", "SHUT OK", 3000); 
        osDelay(2000);
    }
    printf("=== TCP 通道建立成功，移交控制权给 Paho MQTT ===\r\n");
}

void mqttTask(void *argument)
{
    Network network;
    const uint32_t MQTT_PERIOD_MS = 600000;//10分钟事件组触发
    while (1) 
    {
        mqtt_tcpconnect();
        osDelay(200);
        RingBuf_Clear(); 

        NetworkInit(&network);
        MQTTClientInit(&client, &network, 3000, sendbuf, sizeof(sendbuf), readbuf, sizeof(readbuf));

        MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
        connectData.MQTTVersion = 4;                 
        connectData.clientID.cstring = EMQX_CLIENT_ID; 
        connectData.keepAliveInterval = 1200;          
        connectData.cleansession = 1;

        if (MQTTConnect(&client, &connectData) != SUCCESS) 
        {
            printf("[错误] MQTT 协议登录失败!等待2秒后重连...\r\n");
            osDelay(2000);
            continue; 
        }
        MQTTSubscribe(&client, topic_ctrol, 0, messageHandler_subscribe);//订阅iot/device主题
        printf("=== MQTT 协议握手成功!开始循环上报数据 ===\r\n");
        // create_mqtt_timer();//开启循环上报定时器
        for(;;)
        {
            uint32_t flag = osEventFlagsWait(mqtt_event_flags,EVENT_FLAG_ALARM,osFlagsWaitAny,MQTT_PERIOD_MS);
            if(flag == osFlagsErrorTimeout)
            {
                printf("正常10分钟数据上报\r\n");
                MQTT_Time_CallBack(lora_rx_buf);
            }
            else if ((flag & 0x80000000) == 0) 
            {
                if (flag & EVENT_FLAG_ALARM) 
                {
                    printf("收到AI紧急预警抢占发送灾害报文\r\n");
                    MQTT_Time_CallBack(lora_rx_buf);
                }
            }
            
            MQTTYield(&client, 50); 
            osDelay(50);
            if (client.isconnected == 0)
            {
                printf("\r\n[致命] 检测到连接断开或出现脏数据污染！执行网络环境全量重置...\r\n");
                break; 
            }
        }
    }
}

