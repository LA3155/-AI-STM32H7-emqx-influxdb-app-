#include "edge_ai_app.h"
#include <stdint.h>
#include <stdio.h>
#include "farm_ai.h"
#include "farm_ai_data.h"
#include "cmsis_os2.h"
#include "MQTT.h"

static ai_handle farm_ai_handle = AI_HANDLE_NULL;
AI_ALIGNED(4) static ai_u8 activations[AI_FARM_AI_DATA_ACTIVATIONS_SIZE];
static ai_buffer *ai_input  = NULL;
static ai_buffer *ai_output = NULL;
// Python 脚本生成的均值和标准差
const float mean[4]  = {28.73028f, 71.50746f, 5904.59212f, 10.8009f};
const float scale[4] = {5.96746877f, 19.9612672f, 4485.46686312f, 14.61635969f};
extern osMessageQueueId_t loraqueue;

extern threshold_t ai_threshold;
void Edge_AI_Init(void)
{
    ai_error err;

    const ai_handle act_addr[] = { activations };
    err = ai_farm_ai_create_and_init(&farm_ai_handle, act_addr, NULL);

    if (err.type != AI_ERROR_NONE) {
        return;
    }

    ai_input  = ai_farm_ai_inputs_get(farm_ai_handle, NULL);
    ai_output = ai_farm_ai_outputs_get(farm_ai_handle, NULL);
}

// 新增：用于将推理出的概率安全导出给 MQTT 任务，不破坏原有判定结构
float latest_ai_probabilities[4] = {0.0f};

/**
 * @brief  执行推理并返回告警状态
 * @param  input_array: 4个float数据
 * @param  thresholds: 包含4个阈值的数组 (下标1=高温, 2=暴雨, 3=台风)
 */
int ai_calculate(float* input_array, float* thresholds)
{
    ai_i32 batch;
    float output_data[4] = {0};
    float normalized_input[4];

    if (farm_ai_handle == AI_HANDLE_NULL) return 0;

    // 数据标准化
    for(int i = 0; i < 4; i++) {
        normalized_input[i] = (input_array[i] - mean[i]) / scale[i];
    }

    ai_input[0].data = AI_HANDLE_PTR(normalized_input);
    ai_output[0].data = AI_HANDLE_PTR(output_data);

    batch = ai_farm_ai_run(farm_ai_handle, ai_input, ai_output);
    if (batch != 1) return 0;

    //各个预警概率
    for(int k = 0; k < 4; k++) {
        latest_ai_probabilities[k] = output_data[k];
    }

    // 先打印所有原始概率，方便调试观察
    printf("[Edge AI] 概率: [正常:%.2f] [高温:%.2f] [暴雨:%.2f] [台风:%.2f]\r\n", 
           output_data[0], output_data[1], output_data[2], output_data[3]);

    int final_alarm_class = 0;
    float final_alarm_prob = 0.0f;

    // 判断状态, 1(高温), 2(暴雨), 3(台风)
    for(int i = 1; i < 4; i++) {
        if(output_data[i] >= thresholds[i]) {
            if(output_data[i] > final_alarm_prob) {
                final_alarm_prob = output_data[i];
                final_alarm_class = i;
            }
        }
    }

    // 根据独立筛查的结果输出最终判定
    if (final_alarm_class != 0) {
        return final_alarm_class; // 返回对应灾害码
    } else {
        return 0; // 返回 0 表示安全
    }
}

void aiTask(void* argument)
{
    float data[5] = {0};
    while (1)
    {
        if(osMessageQueueGet(loraqueue,data,NULL,osWaitForever) == osOK)
        {
            float ai_thresholds[4] = {0.0f, ai_threshold.high_temp, ai_threshold.storm, ai_threshold.hyphoon};
            int state = ai_calculate(data,ai_thresholds);
            if(state != 0)
            {
                
            }
        }
    }
}