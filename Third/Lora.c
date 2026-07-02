#include "Lora.h"
#include "cmsis_os2.h"
#include "usart.h"
#include <complex.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "MQTT.h"   // 用于通知 MQTT 任务有新数据

// ======================== 绝对内存映射 (规避 H7 BDMA 陷阱) ========================
// 存放解包后的纯净有效数据，锁定在 D3 域 SRAM4 (0x38000000)
// BDMA 不能访问 DTCM (0x20000000)，但可以访问 SRAM4
uint8_t *lora_rx_buf = (uint8_t *)0x38000000;

__attribute__((aligned(32))) uint8_t *lora_raw_buf = (uint8_t *)0x38000200;
// ==================================================================================

// 诊断计数器 (通过调试器或打印查看)
volatile uint32_t lora_irq_count = 0;      // LPUART1 中断触发次数
volatile uint32_t lora_dma_ht_count = 0;   // DMA 半传输完成次数
volatile uint32_t lora_dma_tc_count = 0;   // DMA 传输完成次数
volatile uint32_t lora_idle_count = 0;     // IDLE 中断次数（实际帧接收）
volatile uint32_t lora_crc_ok_count = 0;   // CRC 校验通过次数
volatile uint32_t lora_crc_fail_count = 0; // CRC 校验失败次数

uint8_t lora_flag;
uint8_t lora_size;
uint8_t test12[14];
extern osMessageQueueId_t sizequeue;

/**
 * @brief  初始化 LoRa 接收 (启动 DMA 空闲中断接收)
 */
void LoRa_Init(void)
{
    printf("\r\n[LoRa] ===== LoRa 接收初始化 =====\r\n");
    printf("[LoRa] 接口: LPUART1 (BDMA D3 Domain)\r\n");
    printf("[LoRa] 格式: AA AA [Len=0x0A] [10Bytes Data] [CRC8]\r\n");

    lora_irq_count = 0;
    lora_dma_ht_count = 0;
    lora_dma_tc_count = 0;
    lora_idle_count = 0;
    lora_crc_ok_count = 0;
    lora_crc_fail_count = 0;

    memset(lora_raw_buf, 0, 256);
    memset(lora_rx_buf, 0, 128);

    if (HAL_UARTEx_ReceiveToIdle_DMA(&hlpuart1, lora_raw_buf, 256) != HAL_OK)
    {
        printf("[LoRa] ❌ BDMA 接收启动失败！\r\n");
    }
    else
    {
        printf("[LoRa] ✅ BDMA 空闲接收已启动，等待传感器数据...\r\n");
    }
}

// ... 保留原来的 HAL_UART_RxHalfCpltCallback、HAL_UART_RxCpltCallback ...
// ... 保留原来的 LoRa_HardwareDiag、LoRa_LoopbackTest ...

/**
 * @brief  CRC8 校验函数 (多项式 x^8 + x^2 + x + 1)
 */
uint8_t crc8_calc_shift(uint8_t *data, uint16_t len)
{
    uint8_t crc = 0x00;
    uint8_t poly = 0x07;

    for (uint16_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int bit = 0; bit < 8; bit++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ poly;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}
void lora_prase(uint8_t *data_buf, uint16_t len)
{
    uint8_t *pr = data_buf;
    int i=0;
    for (i=0;i<14;i++) {
        test12[i] = pr[i];
    }
    //帧头检测,数据位检测
    if(pr[0]!=0xAA||pr[1]!=0xAA||len!=14||(pr[2]+4!=len))
    {
        goto exit;
    }
    //CRC校验
    if(crc8_calc_shift(pr,pr[2]+3) != pr[len-1])
    {
        goto exit;
    }
    for (i=0;i<pr[2];i++) {
        lora_rx_buf[i] = pr[3+i];
    }

exit:
    HAL_UARTEx_ReceiveToIdle_DMA(&hlpuart1, lora_raw_buf, 256);
    return;
}

void loraTask(void *argument)
{
    while (1) {
        if(osMessageQueueGet(sizequeue,&lora_size,NULL,osWaitForever) == osOK)
        {
            lora_prase(lora_raw_buf,lora_size);
        }
    }
}