#ifndef __LORA_H__
#define __LORA_H__

#include "main.h"
#include <stdint.h>

// 协议常量定义
#define LORA_HEADER_1       0xAA  // 帧头 1
#define LORA_HEADER_2       0xAA  // 帧头 2
#define LORA_MIN_FRAME_LEN  4     // 最小合法帧长: 头(2) + 长度(1) + 校验(1)

// 外部声明，供给 MQTT.c 或其他任务读取有效数据 (锁定在 D3 域 0x38000000)
extern uint8_t *lora_rx_buf;
extern uint8_t *lora_raw_buf;

// 诊断计数器 (可通过调试器实时查看)
extern volatile uint32_t lora_irq_count;
extern volatile uint32_t lora_dma_ht_count;
extern volatile uint32_t lora_dma_tc_count;
extern volatile uint32_t lora_idle_count;
extern volatile uint32_t lora_crc_ok_count;
extern volatile uint32_t lora_crc_fail_count;

// 函数声明
void LoRa_Init(void);
uint8_t crc8_calc_shift(uint8_t *data, uint16_t len);
void LoRa_HardwareDiag(void);
void LoRa_LoopbackTest(void);
void loraTask(void *argument);

#endif /* __LORA_H__ */