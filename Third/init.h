#ifndef __INIT_H__
#define __INIT_H__

void initTask(void *argument);
#define EVENT_FLAG_ALARM 0x00000001U

typedef struct
{
    float sensor_data[5];        // 最新的温湿度等环境数据
    float probabilities[4];      // 最新的灾害概率
    int alarm_status;            // 最新的报警状态 (0为正常，1/2/3为灾害)
}globaldata_t;

#endif
