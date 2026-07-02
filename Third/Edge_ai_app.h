#ifndef __EDGE_AI_APP_H
#define __EDGE_AI_APP_H

#include "main.h"

void Edge_AI_Init(void);
// 修改为接收 float 数组指针
int Edge_AI_Run_Prediction(float* input_array, float* thresholds);

#endif