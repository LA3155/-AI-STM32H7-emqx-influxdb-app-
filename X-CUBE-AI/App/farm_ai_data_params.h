/**
  ******************************************************************************
  * @file    farm_ai_data_params.h
  * @author  AST Embedded Analytics Research Platform
  * @date    2026-07-02T12:45:56+0800
  * @brief   AI Tool Automatic Code Generator for Embedded NN computing
  ******************************************************************************
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */

#ifndef FARM_AI_DATA_PARAMS_H
#define FARM_AI_DATA_PARAMS_H

#include "ai_platform.h"

/*
#define AI_FARM_AI_DATA_WEIGHTS_PARAMS \
  (AI_HANDLE_PTR(&ai_farm_ai_data_weights_params[1]))
*/

#define AI_FARM_AI_DATA_CONFIG               (NULL)


#define AI_FARM_AI_DATA_ACTIVATIONS_SIZES \
  { 112, }
#define AI_FARM_AI_DATA_ACTIVATIONS_SIZE     (112)
#define AI_FARM_AI_DATA_ACTIVATIONS_COUNT    (1)
#define AI_FARM_AI_DATA_ACTIVATION_1_SIZE    (112)



#define AI_FARM_AI_DATA_WEIGHTS_SIZES \
  { 1344, }
#define AI_FARM_AI_DATA_WEIGHTS_SIZE         (1344)
#define AI_FARM_AI_DATA_WEIGHTS_COUNT        (1)
#define AI_FARM_AI_DATA_WEIGHT_1_SIZE        (1344)



#define AI_FARM_AI_DATA_ACTIVATIONS_TABLE_GET() \
  (&g_farm_ai_activations_table[1])

extern ai_handle g_farm_ai_activations_table[1 + 2];



#define AI_FARM_AI_DATA_WEIGHTS_TABLE_GET() \
  (&g_farm_ai_weights_table[1])

extern ai_handle g_farm_ai_weights_table[1 + 2];


#endif    /* FARM_AI_DATA_PARAMS_H */
