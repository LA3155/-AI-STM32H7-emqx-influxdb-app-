#include "mqtt_port.h"
#include "usart.h" 
#include <stdio.h>
#include <string.h>

// 引入 CMSIS-OS V2 接口以使用信号量
#include "cmsis_os2.h" 

#define RING_BUF_SIZE 2048 
uint8_t ring_buf[RING_BUF_SIZE];
volatile uint16_t ring_head = 0; 
volatile uint16_t ring_tail = 0; 

// 新增：用于事件驱动的二值信号量
osSemaphoreId_t mqtt_rx_sem = NULL;

void RingBuf_Write(uint8_t *data, uint16_t len) {
    for (uint16_t i = 0; i < len; i++) {
        ring_buf[ring_head] = data[i];
        ring_head = (ring_head + 1) % RING_BUF_SIZE;
    }
    
    // 【事件触发】：每当 DMA 写入了新数据，立刻释放信号量
    // 这将瞬间唤醒正在 mqtt_port_read 中睡眠等待的 Paho 任务
    if (mqtt_rx_sem != NULL) {
        osSemaphoreRelease(mqtt_rx_sem);
    }
}

void RingBuf_Clear(void) {
    __disable_irq();
    ring_tail = ring_head;
    __enable_irq();
}

int RingBuf_Read(uint8_t *data, uint16_t len) {
    int count = 0;
    while (count < len && ring_tail != ring_head) {
        data[count++] = ring_buf[ring_tail];
        ring_tail = (ring_tail + 1) % RING_BUF_SIZE;
    }
    return count;
}

void TimerInit(Timer* timer) {
    timer->end_time = 0;
}

char TimerIsExpired(Timer* timer) {
    return (osKernelGetTickCount() >= timer->end_time) ? 1 : 0;
}

void TimerCountdownMS(Timer* timer, unsigned int ms) {
    timer->end_time = osKernelGetTickCount() + ms;
}

void TimerCountdown(Timer* timer, unsigned int seconds) {
    TimerCountdownMS(timer, seconds * 1000);
}

int TimerLeftMS(Timer* timer) {
    int32_t left = timer->end_time - osKernelGetTickCount();
    return (left < 0) ? 0 : left;
}

int mqtt_port_write(Network* n, unsigned char* buffer, int len, int timeout_ms) {
    HAL_StatusTypeDef status = HAL_UART_Transmit(&huart5, buffer, len, timeout_ms);
    // 透传模式下模块仍可能生成 SEND OK 等文本，
    // 等待 DMA 抓完，冲掉环形缓冲，防止 Paho 读到脏数据
    osDelay(100);
    RingBuf_Clear();
    if (status == HAL_OK) return len;
    return 0;
}

int mqtt_port_read(Network* n, unsigned char* buffer, int len, int timeout_ms) {
    uint32_t start_time = osKernelGetTickCount();
    int read_len = 0;
    
    while (read_len < len) {
        // 先尝试从环形缓冲区捞取数据
        int chunk = RingBuf_Read(buffer + read_len, len - read_len);
        read_len += chunk;
        
        if (read_len == len) {
            break; // 数据已凑齐，直接退出
        }
        
        uint32_t elapsed = osKernelGetTickCount() - start_time;
        if (elapsed >= timeout_ms) {
            break; // 已达到 Paho 指定的超时时间，退出
        }
        
        // 【终极重构】：彻底告别 osDelay(1) 轮询
        // 计算剩余的超时时间，并挂起当前任务。
        // CPU 占用率降至 0%，直到 RingBuf_Write 释放信号量将其唤醒。
        osSemaphoreAcquire(mqtt_rx_sem, timeout_ms - elapsed); 
    }
    return read_len; 
}

void NetworkInit(Network* n) {
    n->mqttread = mqtt_port_read;
    n->mqttwrite = mqtt_port_write;
    
    // 初始化二值信号量 (最大值为1，初始值为0)
    if (mqtt_rx_sem == NULL) {
        mqtt_rx_sem = osSemaphoreNew(1, 0, NULL);
    }
}