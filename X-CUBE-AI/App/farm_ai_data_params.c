/**
  ******************************************************************************
  * @file    farm_ai_data_params.c
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

#include "farm_ai_data_params.h"


/**  Activations Section  ****************************************************/
ai_handle g_farm_ai_activations_table[1 + 2] = {
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
  AI_HANDLE_PTR(NULL),
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
};




/**  Weights Section  ********************************************************/
AI_ALIGNED(32)
const ai_u64 s_farm_ai_weights_array_u64[168] = {
  0x3e3be0b3be98d703U, 0xbf289df3bdcf4d44U, 0xbdcf0306bed2a7c7U, 0xbd21c5773e99f74bU,
  0xbf35d382be677ef8U, 0xbf2a3f2e3e2a5f1bU, 0x3f12b438bf04f54fU, 0x3dfda2d03e29a1c5U,
  0xbef83a78bedd2435U, 0xbe1600d4bc694eb9U, 0x3ed48ff2be63a312U, 0xbe8165553e1c2880U,
  0x3ece7b533e467c23U, 0xbbbb4dc5bf1dc1d8U, 0xbe7c821bbd5ddc0eU, 0x3f56c45a3ef00e28U,
  0x3ef42708bedea4d1U, 0x3f51f1debef1c5e6U, 0xbeae89b23ec1381dU, 0x3e89c0533eb810a6U,
  0xbe1c8955beb0562cU, 0xbf12c1febecc0fc8U, 0x3f066a573e9a4aafU, 0x3ed0b954bf28b45bU,
  0xbf58d9e7bd68d220U, 0xbee5d9f13e70aa79U, 0x3f3a576bbf3eb670U, 0x3f0376efbf1f593dU,
  0x3ef56e62bee92912U, 0x3f8047aa3ed5c006U, 0xbf038e933f0a5af8U, 0x3e7cf9c53ec76f95U,
  0x3cc54f633edfa40cU, 0x3c8b33ec3ea83fceU, 0x3e7d580e3e88e844U, 0x3eb1d0313e5187f0U,
  0xbdf2c7173e527d53U, 0x3e7774183ef17a72U, 0x3e84aa3a3ece5a55U, 0xbe1790e63e292221U,
  0xbeac7a71be62ae79U, 0x3f46c1e4bc7c0effU, 0xbe30da043da214b6U, 0x3f4c24a23f3dd918U,
  0x3d14a5073f232d78U, 0x3f38aac2bedc68abU, 0x3f134bab3b0c6d81U, 0xbe93b8793ebe7f05U,
  0xbc33a19b3f3aedf1U, 0x3bcf00aa3f1234f3U, 0x3f5897edbd56a292U, 0x3bb135cd3e32f58fU,
  0xbebb15f7be8a1f95U, 0x3e9b79913f472b8eU, 0x3bfcaa68bd765b86U, 0xbecce06dbe8fb947U,
  0x3ef92c003f0954fbU, 0x3d6e4afe3f1f01d5U, 0x3f0cf8143eedea61U, 0xbf09a078be0ecd24U,
  0xbf43efe3beb22190U, 0x3d902c5d3f5cfbabU, 0x3e885eca3da80a54U, 0x3b3028313cf8bb84U,
  0xbd1294463f4cd416U, 0xbeb8a3153f2d612cU, 0x3f434d053e9ba847U, 0x3ec7407bbe3c1687U,
  0x3ef0b809bf1a66fcU, 0xbf04cd583eddfbedU, 0xbf1511103f4f6888U, 0xbd1ee976be434118U,
  0x3e47fe3a3d9c6d93U, 0x3bd998273efae832U, 0x3deae182bdcc9815U, 0x3e3e6982bed0ac4dU,
  0x3e2743a53e97cc62U, 0x3e873963beba147aU, 0xbe1d7d863e923007U, 0x3ef2f6ddbeb8cf6cU,
  0xbe4df930be225310U, 0x3d9c43d1be82148cU, 0x3f0648703ddc78e2U, 0x3e07dadd3f2762daU,
  0x3f3e8289bc860ddbU, 0x3be61b413e8e140bU, 0x3e618336be4adca9U, 0x3f0b962bbf21ed9cU,
  0xbea190bebf878e50U, 0x3e55f4ef3e98c30cU, 0xbf259c04becd9517U, 0x3f25bc6ebec27cdfU,
  0x3ea44f09be66b015U, 0x3dd3a1f4befdcdd9U, 0xbcc18f393ed7ec3cU, 0x3ec7f0a8be458845U,
  0xbe2c619b3f4aa248U, 0x3d6ad2023d48d0d8U, 0x3f3fe5edbf2fba25U, 0xbecfbaeb3ef6b08bU,
  0x3e9bdfe6bdf44a04U, 0x3f071d8a3e7413a8U, 0x3edb594ebe8b6554U, 0xbdeb9d0fbf5f6757U,
  0xbeb59580bf366b88U, 0xbed99445bdd7b088U, 0xba4f7af83e007f5cU, 0x3f1b92093d8981c6U,
  0x3f4cf959beac5e31U, 0xbed9b3b0beed7885U, 0xbe93e20ebd25f418U, 0x3f40dca73db3e825U,
  0x3f0129d6be4d201aU, 0xbdc62a793f0b88eaU, 0x3e616c773f229909U, 0x3ec4b084be534125U,
  0xbf3aed01bdce8cdaU, 0x3e4781b8be04725cU, 0x3e2e51243de967deU, 0xbe80a0683e9c287bU,
  0xbd0d8d763df78690U, 0x3e9a87ebbf0caae0U, 0x3f57a7e3bd91d52cU, 0xbe3e46973f47d303U,
  0x3ec28003bdbef58bU, 0x3e91f1613f4e32a2U, 0xbe01af24befea6a9U, 0x3e647dbcbef43c8fU,
  0x3e009761befd8bacU, 0xbee0ed753e7474e2U, 0xbe9408243c30e3ecU, 0x3e31bdac3e529d61U,
  0x3e59031b3dcc5e83U, 0xbee0c252bee8bc30U, 0xbdce80a1be4f0405U, 0xbe2358c23e8bb39aU,
  0x3e95b2f53e3a027aU, 0x3e8662113e242eb9U, 0x3d8580f5bb57ac31U, 0x3cf61c6e3dda177bU,
  0x3e50c7873d2ad590U, 0xbdedd7673d45f246U, 0x3f258815bed775e2U, 0x3f323fba3f66a9c1U,
  0xbef801ce3e9dc9e8U, 0xbcdfb167bf4e0978U, 0x3f13f302bf7d78aeU, 0x3e27b311bf99bf79U,
  0xbf79300fbeb1a553U, 0x3ed2412fbecaf209U, 0x3f3604dd3f143d38U, 0xbec328223e1d3c29U,
  0xbe26bf263f02b44fU, 0x3e0696a2bf039490U, 0x3f28fd6cbd034909U, 0xbf53d60e3f0152bcU,
  0x3f0acd253e24ba84U, 0x3f021012bf213275U, 0xbe8458533e91ce1aU, 0x3e859eb13f29020fU,
  0x3de342763f3fb34fU, 0xbeecad4dbf40e073U, 0x3da1e2b8bf174573U, 0x3da5a814bf2b3312U,
  0x3d9c4bce3e1b2052U, 0x3da47556bf6fe1c5U, 0x3dc185083d55e2f1U, 0x3dcf952fbe457ed1U,
};


ai_handle g_farm_ai_weights_table[1 + 2] = {
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
  AI_HANDLE_PTR(s_farm_ai_weights_array_u64),
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
};

