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

 #ifndef GE_OP_MVN_OPS_H
 #define GE_OP_MVN_OPS_H

 #include "graph/operator_reg.h"

 namespace ge {
/**
*@brief Normalizes the input.

*@par Inputs:
* One input:
*x: An NCHW tensor of type float16 or float32.

*@par Attributes:
*@li normalize_variance: An optional bool specifying whether to normalize the variance, either "true" (default) or "false".
*@li across_channels: An optional bool specifying whether to perform across-channel MVN, either "true" or "false" (default).
*@li eps: An optional float32 epsilon for not dividing by zero. Defaults to "1e-9".

*@par Outputs:
*y: An NCHW tensor of type float16 or float32.

*@attention Constraints:\n
* The input tensor must have the NCHW format, whose shape length must be 4.
*/

 REG_OP(MVN)
     .INPUT(x, TensorType({DT_FLOAT, DT_FLOAT16})) /* "First operand." */
     .OUTPUT(y, TensorType({DT_FLOAT, DT_FLOAT16}))  /* "Result, has same element type as inputs" */
     .ATTR(normalize_variance, Bool, true)
     .ATTR(across_channels, Bool, false)
     .ATTR(eps, Float, 1e-9)
     .OP_END_FACTORY_REG(MVN)
 } // namespace ge

 #endif // GE_OP_MVN_OPS_H
