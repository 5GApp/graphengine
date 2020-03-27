/**
 * Copyright 2019-2020 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

 #ifndef GE_OP_NORMALIZE_OPS_H
 #define GE_OP_NORMALIZE_OPS_H

 #include "graph/operator_reg.h"

 namespace ge {

 REG_OP(Normalize)
     .INPUT(x1, TensorType({DT_FLOAT16, DT_FLOAT, DT_INT8}))
     .INPUT(x2, TensorType({DT_FLOAT16, DT_FLOAT, DT_INT8}))
     .OUTPUT(y, TensorType({DT_FLOAT16, DT_FLOAT, DT_INT8}))
     .ATTR(across_spatial, Bool, true)
     .ATTR(channel_shared, Bool, true)
     .ATTR(eps, Float, 1e-10)
     .OP_END_FACTORY_REG(Normalize);

 REG_OP(NormalizeSum)
     .INPUT(x1, TensorType({DT_FLOAT16, DT_FLOAT, DT_INT8}))
     .OUTPUT(y, TensorType({DT_FLOAT16, DT_FLOAT}))
     .ATTR(across_spatial, Bool, true)
     .OP_END_FACTORY_REG(NormalizeSum);

 REG_OP(NormalizeScale)
     .INPUT(x1, TensorType({DT_FLOAT16, DT_FLOAT, DT_INT8}))
     .INPUT(x2, TensorType({DT_FLOAT16, DT_FLOAT, DT_INT8}))
     .INPUT(x3, TensorType({DT_FLOAT16, DT_FLOAT}))
     .OUTPUT(y, TensorType({DT_FLOAT16, DT_FLOAT, DT_INT8}))
     .ATTR(across_spatial, Bool, true)
     .ATTR(channel_shared, Bool, true)
     .ATTR(eps, Float, 1e-10)
     .OP_END_FACTORY_REG(NormalizeScale);

 } // namespace ge

 #endif // GE_OP_NORMALIZE_OPS_H
