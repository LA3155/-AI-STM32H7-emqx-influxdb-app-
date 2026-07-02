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

//10分钟定时器上报逻辑
osTimerId_t MQTT_TimeHandle;
uint32_t mqtt_time = 600000;

//定义函数指针
typedef void (*messageHandler_subscribe_t)(MessageData*);
typedef void (*mqttdata_t)(void *);

volatile uint8_t mqtt_flag;

void MQTT_Time_CallBack(void *pr)
{
    uint8_t *buf = (uint8_t *)pr;
    uint16_t raw_temp  = (buf[0] << 8) | buf[1];//温度
    uint16_t raw_humi  = (buf[2] << 8) | buf[3];//湿度
    uint16_t raw_light = (buf[4] << 8) | buf[5];//光照
    uint16_t raw_press = (buf[6] << 8) | buf[7];//气压
    uint16_t raw_wind  = (buf[8] << 8) | buf[9];//风速

    int temp_int = raw_temp / 10, temp_dec = raw_temp % 10;
    int humi_int = raw_humi / 10, humi_dec = raw_humi % 10;
    int light_int = raw_light / 10, light_dec = raw_light % 10;
    int press_int= raw_press / 10, press_dec = raw_press % 10;
    int wind_int = raw_wind / 10, wind_dec = raw_wind % 10;

    ai_input[0] = raw_temp / 10.0f;
    ai_input[1] = raw_humi / 10.0f;
    ai_input[2] = raw_light/ 10.0f;
    ai_input[3] = raw_wind / 10.0f;

    float ai_thresholds[4] = {0.0f, ai_threshold.high_temp, ai_threshold.storm, ai_threshold.hyphoon};
    // int alarm_status = Edge_AI_Run_Prediction(ai_input, ai_thresholds);

    // 提取导出的概率值，放大100倍转换为整数，安全推送给 APP 
    int p_temp  = (int)(latest_ai_probabilities[1] * 100);
    int p_storm = (int)(latest_ai_probabilities[2] * 100);
    int p_wind  = (int)(latest_ai_probabilities[3] * 100);
    
    // sprintf(json_payload, 
    // "{\"temp\": %d.%d, \"humi\": %d.%d, \"light\": %d.%d, \"press\": %d.%d, \"wind\": %d.%d, \"status\": %d, \"p1\": %d, \"p2\": %d, \"p3\": %d}", 
    // temp_int, temp_dec, humi_int, humi_dec, light_int,light_dec, press_int, press_dec,wind_int, wind_dec,alarm_status,p_temp,p_storm,p_wind);
                
    MQTTMessage message;
    message.qos = QOS0;             
    message.retained = 0;           
    message.dup = 0;
    message.id = 0;
    message.payload = (void*)json_payload;
    message.payloadlen = strlen(json_payload); 

    // if (MQTTPublish(&client, "iot/environment", &message) == SUCCESS)
    // {
    //     printf("[MQTT Pub] 成功: %s\r\n", json_payload);
    // }
    // else
    // {
    //     printf("[MQTT Pub] 发送失败！\r\n");
    // }
}

void mqtt_flagtuggole(void *argument)
{
    mqtt_flag = 1;
}

void create_mqtt_timer(void)
{
    if (MQTT_TimeHandle == NULL)
    {
        MQTT_TimeHandle = osTimerNew
        (
            mqtt_flagtuggole,
            osTimerPeriodic,
            lora_rx_buf,
            NULL
        );
    }
    if(MQTT_TimeHandle != NULL)
    {
        osTimerStart(MQTT_TimeHandle,mqtt_time);
    }
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
    // char json_payload[256];
    // ==================== 预置测试数据 ====================
    lora_rx_buf[0] = 0x01; lora_rx_buf[1] = 0x40; //温度
    lora_rx_buf[2] = 0x02; lora_rx_buf[3] = 0x58; //湿度
    lora_rx_buf[4] = 0x27; lora_rx_buf[5] = 0x10; //光照
    lora_rx_buf[6] = 0x00; lora_rx_buf[7] = 0x00; //海拔
    lora_rx_buf[8] = 0x00; lora_rx_buf[9] = 0x0A; //风速
    // //边缘AI初始化
    // Edge_AI_Init();
    
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
        // RingBuf_Clear();
        MQTTSubscribe(&client, topic_ctrol, 0, messageHandler_subscribe);//订阅iot/device主题
        printf("=== MQTT 协议握手成功!开始循环上报数据 ===\r\n");
        create_mqtt_timer();//开启循环上报定时器
        for(;;)
        {
            MQTTYield(&client, 50); 
            if(mqtt_flag)
            {
                MQTT_Time_CallBack(lora_rx_buf);
                mqtt_flag = 0;
            }
            osDelay(50);
            if (client.isconnected == 0)
            {
                printf("\r\n[致命] 检测到连接断开或出现脏数据污染！执行网络环境全量重置...\r\n");
                break; 
            }
        }
    }
}

