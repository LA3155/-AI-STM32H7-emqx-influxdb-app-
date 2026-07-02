#include "edge_ai_app.h"
#include <stdint.h>
#include <stdio.h>
#include "farm_ai.h"
#include "farm_ai_data.h"

static ai_handle farm_ai_handle = AI_HANDLE_NULL;
AI_ALIGNED(4) static ai_u8 activations[AI_FARM_AI_DATA_ACTIVATIONS_SIZE];
static ai_buffer *ai_input  = NULL;
static ai_buffer *ai_output = NULL;
// Python 脚本生成的均值和标准差
const float mean[4]  = {28.73028f, 71.50746f, 5904.59212f, 10.8009f};
const float scale[4] = {5.96746877f, 19.9612672f, 4485.46686312f, 14.61635969f};
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
 * @brief  执行推理并返回告警状态 (独立多通道判定版)
 * @param  input_array: 4个float数据
 * @param  thresholds: 包含4个阈值的数组 (下标1=高温, 2=暴雨, 3=台风)
 */
int Edge_AI_Run_Prediction(float* input_array, float* thresholds)
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

    for(int k = 0; k < 4; k++) {
        latest_ai_probabilities[k] = output_data[k];
    }

    // 先打印所有原始概率，方便调试观察
    printf("[Edge AI] 概率: [正常:%.2f] [高温:%.2f] [暴雨:%.2f] [台风:%.2f]\r\n", 
           output_data[0], output_data[1], output_data[2], output_data[3]);

    // ==========================================================
    // 核心修改：独立阈值判定逻辑 (打破赢家通吃，实现绝对越限报警)
    // ==========================================================
    int final_alarm_class = 0;
    float final_alarm_prob = 0.0f;

    // 跳过状态0(正常)，直接独立审查 1(高温), 2(暴雨), 3(台风)
    for(int i = 1; i < 4; i++) {
        // 只要该灾害的概率 >= 你针对该灾害设定的独立阈值，就记录下来
        if(output_data[i] >= thresholds[i]) {
            // 如果同时有多个灾害超过阈值，取概率更大的那个去报警
            if(output_data[i] > final_alarm_prob) {
                final_alarm_prob = output_data[i];
                final_alarm_class = i;
            }
        }
    }

    // 根据独立筛查的结果输出最终判定
    if (final_alarm_class != 0) {
        printf(" -> 🚨 强制越限告警! 状态码: %d (当前概率: %d%%, 已达到预设阈值: %d%%)\r\n", 
               final_alarm_class, (uint8_t)(final_alarm_prob * 100), (uint8_t)(thresholds[final_alarm_class] * 100));
        return final_alarm_class; // 返回对应灾害码
    } else {
        // 没有任何一个灾害超过它们各自的阈值
        printf(" -> 🟢 判定为安全状态 (各异常概率均未越过预设警戒线)\r\n");
        return 0; // 返回 0 表示安全
    }
}