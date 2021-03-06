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

#ifndef GE_OP_TRAINING_OPS_H
#define GE_OP_TRAINING_OPS_H

#include "../graph/operator_reg.h"
namespace ge {
/**
*@brief Updates "var" according to the AdaMax algorithm.\n
*  t-1 mean previous period.
*  m_t <- beta1 * m{t-1} + (1 - beta1) * grad\n
*  v_t <- max(beta2 * v{t-1}, abs(grad))\n
*  var <- var - lr / (1 - beta1^t) * m_t / (v_t + epsilon)
*
*@attention Constraints:\n
*  the input tensors must have the same shape.
*
*@par Inputs:
*@li var: A mutable tensor. Must be one of the following types: TensorType::NumberType().
*     Should be from a Variable().
*@li m: A mutable tensor. Has the same type as "var".
*     Should be from a Variable().
*@li v: A mutable tensor. Has the same type as "var".
*     Should be from a Variable().
*@li beta1_power: A scalar. Has the same type as "var".
*@li lr: learning_rate. A scalar. Has the same type as "var".
*@li beta1: A scalar. Has the same type as "var".
*@li beta2: A scalar. Has the same type as "var".
*@li epsilon: A scalar. Has the same type as "var".
*@li grad: A tensor for the gradient. Has the same type as "var". 
*
*@par Attributes:\n
* use_locking: An optional bool. Defaults to "False".
*     If "True", updating of the "var", "ms", and "mom" tensors is protected
*     by a lock; otherwise the behavior is undefined, but may exhibit less
*     contention.
*
*@par Outputs:
* var: A mutable tensor. Has the same type as input "var".
*
*/
REG_OP(ApplyAdaMax)
    .INPUT(var, TensorType::NumberType())
    .INPUT(m, TensorType::NumberType())
    .INPUT(v, TensorType::NumberType())
    .INPUT(beta1_power, TensorType::NumberType())
    .INPUT(lr, TensorType::NumberType())
    .INPUT(beta1, TensorType::NumberType())
    .INPUT(beta2, TensorType::NumberType())
    .INPUT(epsilon, TensorType::NumberType())
    .INPUT(grad, TensorType::NumberType())
    .OUTPUT(var, TensorType::NumberType())
    .ATTR(use_locking, Bool, false)
    .OP_END_FACTORY_REG(ApplyAdaMax)

/**
*@brief Updates "var" according to the momentum scheme. Set use_nesterov = True if you
*   want to use Nesterov momentum.\n
*  computing process: \n
*  accum = accum * momentum + grad\n
*  var -= lr * accum
*
*@attention Constraints:\n
*  the input tensors must have the same shape.
*
*@par Inputs:
*@li var: A mutable tensor. Should be from a Variable().
*@li accum: A mutable tensor. Has the same type as "var".
*     Should be from a Variable().
*@li lr: A scalar. Has the same type as "var".
*@li grad: A tensor for the gradient. Has the same type as "var".
*
*@par Attributes:
*@li use_nesterov: An optional bool. Defaults to "False".
*     If "True", the tensor passed to compute grad will be 
*     var - lr * momentum * accum, so in the end, the var you get is actually 
*     var - lr * momentum * accum.
*
*@li use_locking: An optional bool. Defaults to "False".\n
*     If "True", updating of the "var", "ms", and "mom" tensors is protected by a lock;
*     otherwise the behavior is undefined, but may exhibit less contention.
*
*@par Outputs:
* var: A mutable tensor. Has the same type as input "var".
*
*/

REG_OP(ApplyMomentum)
    .INPUT(var, TensorType::NumberType())
    .INPUT(accum, TensorType::NumberType())
    .INPUT(lr, TensorType::NumberType())
    .INPUT(grad, TensorType::NumberType())
    .INPUT(momentum, TensorType::NumberType())
    .OUTPUT(var, TensorType::NumberType())
    .ATTR(use_nesterov, Bool, false)
    .ATTR(use_locking, Bool, false)
    .OP_END_FACTORY_REG(ApplyMomentum)

REG_OP(ApplyMomentumCCE)
    .INPUT(var, TensorType::NumberType())
    .INPUT(accum, TensorType::NumberType())
    .INPUT(lr, TensorType::NumberType())
    .INPUT(grad, TensorType::NumberType())
    .INPUT(momentum, TensorType::NumberType())
    .OUTPUT(var, TensorType::NumberType())
    .ATTR(use_nesterov, Bool, false)
    .ATTR(use_locking, Bool, false)
    .OP_END_FACTORY_REG(ApplyMomentumCCE)

/**
*@brief Updates "var" according to the AddSign update.\n
*  t-1 mean previous period.
*  m_t <- beta1 * m_{t-1} + (1 - beta1) * grad\n
*  update <- exp(logbase * sign_decay * sign(grad) * sign(m_t)) * grad\n
*  var <- var - lr * update
*
*@attention Constraints:\n
*  the input tensors must have the same shape.
*
*@par Inputs:
*@li var: A mutable tensor. Should be from a Variable().
*@li m: A mutable tensor. Has the same type as "var".
*     Should be from a Variable().
*@li lr: A scalar. Has the same type as "var".
*@li logbase: A scalar. Has the same type as "var".
*@li sign_decay: A scalar. Has the same type as "var".
*@li beta: A scalar. Has the same type as "var".
*@li grad: A tensor for the gradient. Has the same type as "var".
*
*@par Attributes:
* use_locking: An optional bool. Defaults to "False".
*     If "True", updating of the "var", "ms", and "mom" tensors is protected
*     by a lock; otherwise the behavior is undefined, but may exhibit less
*     contention.
*
*@par Outputs:
* var: A mutable tensor. Has the same type as input "var".
*
*/
REG_OP(ApplyPowerSign)
    .INPUT(var, TensorType::NumberType())
    .INPUT(m, TensorType::NumberType())
    .INPUT(lr, TensorType::NumberType())
    .INPUT(logbase, TensorType::NumberType())
    .INPUT(sign_decay, TensorType::NumberType())
    .INPUT(beta, TensorType::NumberType())
    .INPUT(grad, TensorType::NumberType())
    .OUTPUT(var, TensorType::NumberType())
    .ATTR(use_locking, Bool, false)
    .OP_END_FACTORY_REG(ApplyPowerSign)

/**
*@brief Updates "var" as FOBOS algorithm with fixed learning rate.\n
*  prox_v = var - alpha * delta\n
*  var = sign(prox_v)/(1+alpha*l2) * max{|prox_v|-alpha*l1,0}
*
*@attention Constraints:\n
*  the input tensors must have the same shape.
*
*@par Inputs:
*@li var: A mutable tensor. Should be from a Variable().
*@li alpha: A scalar. Has the same type as "var".
*@li l1: A scalar. Has the same type as "var".
*@li l2: A scalar. Has the same type as "var".
*@li delta: A tensor. Has the same type as "var". The change.
*
*@par Attributes:
* use_locking: An optional bool. Defaults to "False".
*     If "True", updating of the "var", "ms", and "mom" tensors is protected
*     by a lock; otherwise the behavior is undefined, but may exhibit less
*     contention.
*
*@par Outputs:
* var: A mutable tensor. Has the same type as input "var".
*
*/
REG_OP(ApplyProximalGradientDescent)
    .INPUT(var, TensorType::NumberType())
    .INPUT(alpha, TensorType::NumberType())
    .INPUT(l1, TensorType::NumberType())
    .INPUT(l2, TensorType::NumberType())
    .INPUT(delta, TensorType::NumberType())
    .OUTPUT(var, TensorType::NumberType())
    .ATTR(use_locking, Bool, false)
    .OP_END_FACTORY_REG(ApplyProximalGradientDescent)

/**
*@brief Updates "var" according to the AddSign update.

*@par Inputs:
*Seven inputs, including:
* @li var: A mutable Tensor of type TensorType::NumberType().
*     Should be a Variable Tensor.
* @li m: A mutable Tensor of the same type as "var".
*     Should be a Variable Tensor.
* @li lr: A Tensor of the same type as "var", for the scaling factor. Must be a scalar.
* @li alpha: A Tensor of the same type as "var". Must be a scalar.
* @li sign_decay: A Tensor of the same type as "var". Must be a scalar.
* @li beta: A Tensor of the same type as "var". Must be a scalar.
* @li grad: A Tensor of the same type as "var", for the gradient.


*@par Attributes:
*use_locking: An optional bool. Defaults to "False".
*     If "True", updating of the "var" and "m" tensors will be
*     protected by a lock; otherwise the behavior is undefined,
*     but may exhibit less contention.

*@par Outputs:
*var: A mutable Tensor. Has the same type as "var".
*/
REG_OP(ApplyAddSign)
    .INPUT(var, TensorType::NumberType())
    .INPUT(m, TensorType::NumberType())
    .INPUT(lr, TensorType::NumberType())
    .INPUT(alpha, TensorType::NumberType())
    .INPUT(sign_decay, TensorType::NumberType())
    .INPUT(beta, TensorType::NumberType())
    .INPUT(grad, TensorType::NumberType())
    .OUTPUT(var, TensorType::NumberType())
    .ATTR(use_locking, Bool, false)
    .OP_END_FACTORY_REG(ApplyAddSign)

/**
*@brief Updates "var" according to the centered RMSProp algorithm.\n
*  The centered RMSProp algorithm uses an estimate of the centered second moment
*  (i.e., the variance) for normalization, as opposed to regular RMSProp, which
*  uses the (uncentered) second moment. This often helps with training, but is
*  slightly more expensive in terms of computation and memory.
*
*  t-1 mean previous period.
*  mg <- rho * mg{t-1} + (1-rho) * grad\n
*  ms <- rho * ms{t-1} + (1-rho) * grad * grad\n
*  mom <- momentum * mom{t-1} + lr * grad / sqrt(ms - mg * mg + epsilon)\n
*  var <- var - mom\n
*
*@attention Constraints:\n
*@li in dense implementation of this algorithm, mg, ms, and mom will
*    update even if the grad is zero, but in this sparse implementation, mg, ms,
*    and mom will not update in iterations during which the grad is zero.
*@li the input tensors must have the same shape.
*
*@par Inputs:
*@li var: A mutable tensor. Should be from a Variable().
*@li mg: A mutable tensor. Has the same type as "var".
*     Should be from a Variable().
*@li ms: A mutable tensor. Has the same type as "var".
*     Should be from a Variable().
*@li mom: A mutable tensor. Has the same type as "var".
*     Should be from a Variable().
*@li lr: A scalar. Has the same type as "var".
*@li rho: A scalar. Has the same type as "var".
*@li momentum: A tensor. Has the same type as "var".
*@li epsilon: A scalar. Has the same type as "var".
*@li grad: A tensor for the gradient. Has the same type as "var".
*
*@par Attributes:
* use_locking: An optional bool. Defaults to "False".
*     If "True", updating of the "var", "ms", and "mom" tensors is protected
*     by a lock; otherwise the behavior is undefined, but may exhibit less
*     contention.
*
*@par Outputs:
* var: A mutable tensor. Has the same type as input "var".
*
*/
REG_OP(ApplyCenteredRMSProp)
    .INPUT(var, TensorType::NumberType())
    .INPUT(mg, TensorType::NumberType())
    .INPUT(ms, TensorType::NumberType())
    .INPUT(mom, TensorType::NumberType())
    .INPUT(lr, TensorType::NumberType())
    .INPUT(rho, TensorType::NumberType())
    .INPUT(momentum, TensorType::NumberType())
    .INPUT(epsilon, TensorType::NumberType())
    .INPUT(grad, TensorType::NumberType())
    .OUTPUT(var, TensorType::NumberType())
    .ATTR(use_locking, Bool, false)
    .OP_END_FACTORY_REG(ApplyCenteredRMSProp)

/**
*@brief Updates "var" by subtracting 'alpha' * 'delta' from it.\n
*   var -= delta * alpha
*
*@attention Constraints:\n
*  the input tensors must have the same shape.
*
*@par Inputs:
*@li var: A mutable tensor. Should be from a Variable().
*@li alpha: A scalar. Has the same type as "var".
*@li delta: A tensor for the change. Has the same type as "var".
*
*@par Attributes:
* use_locking: An optional bool. Defaults to "False".
*     If "True", updating of the "var", "ms", and "mom" tensors is protected
*     by a lock; otherwise the behavior is undefined, but may exhibit less
*     contention.
*
*@par Outputs:
* var: A mutable tensor. Has the same type as input "var".
*
*/
REG_OP(ApplyGradientDescent)
    .INPUT(var, TensorType::NumberType())
    .INPUT(alpha, TensorType::NumberType())
    .INPUT(delta, TensorType::NumberType())
    .OUTPUT(var, TensorType::NumberType())
    .ATTR(use_locking, Bool, false)
    .OP_END_FACTORY_REG(ApplyGradientDescent)

/**
*@brief Updates "var" according to the adagrad scheme.\n
*   accum += grad * grad\n
*   var -= lr * grad * (1 / sqrt(accum))
*
*@attention Constraints:\n
*  the input tensors must have the same shape.
*
*@par Inputs:
*@li var: A mutable tensor. Should be from a Variable().
*@li accum: A mutable tensor. Has the same type as "var".
*     Should be from a Variable().
*@li lr: A scalar. Has the same type as "var".
*@li grad: A tensor for the gradient. Has the same type as "var".
*
*@par Attributes:
* use_locking: An optional bool. Defaults to "False".
*     If "True", updating of the "var", "ms", and "mom" tensors is protected
*     by a lock; otherwise the behavior is undefined, but may exhibit less
*     contention.
*
*@par Outputs:
* var: A mutable tensor. Has the same type as input "var".
*
*/
REG_OP(ApplyAdagrad)
    .INPUT(var, TensorType::NumberType())
    .INPUT(accum, TensorType::NumberType())
    .INPUT(lr, TensorType::NumberType())
    .INPUT(grad, TensorType::NumberType())
    .OUTPUT(var, TensorType::NumberType())
    .ATTR(update_slots, Bool, true)
    .ATTR(use_locking, Bool, false)
    .OP_END_FACTORY_REG(ApplyAdagrad)

/**
*@brief Updates "var" according to the proximal adagrad scheme.

*@par Inputs:
*Eight inputs, including:
* @li var: A mutable Tensor. Must be one of the following types:
*     TensorType::NumberType(). Should be a Variable Tensor.
* @li gradient_accumulator: A mutable Tensor. Must have the same
*     type as "var". Should be a Variable Tensor.
* @li gradient_squared_accumulator: A mutable Tensor of the same type as "var".
*     Should be a Variable Tensor.
* @li grad: A Tensor of the same type as "var", for the gradient.
* @li lr: A Tensor of the same type as "var".
*     Scaling factor. Must be a scalar.
* @li l1: A Tensor of the same type as "var".
*     L1 regulariation. Must be a scalar.
* @li l2: A Tensor of the same type as "var".
*     L2 regulariation. Must be a scalar.
* @li global_step: A Tensor of type int32 or int64.
*     Training step number. Must be a scalar.

*@par Attributes:
*use_locking: An optional bool. Defaults to "False".
*     If "True", updating of the var and accum tensors will be
*     protected by a lock; otherwise the behavior is undefined,
*     but may exhibit less contention.

*@par Outputs:
*var: A mutable Tensor. Has the same type as "var".
*/
REG_OP(ApplyAdagradDA)
    .INPUT(var, TensorType::NumberType())
    .INPUT(gradient_accumulator, TensorType::NumberType())
    .INPUT(gradient_squared_accumulator, TensorType::NumberType())
    .INPUT(grad, TensorType::NumberType())
    .INPUT(lr, TensorType::NumberType())
    .INPUT(l1, TensorType::NumberType())
    .INPUT(l2, TensorType::NumberType())
    .INPUT(global_step, TensorType({DT_INT32, DT_INT64}))
    .OUTPUT(var, TensorType::NumberType())
    .ATTR(use_locking, Bool, false)
    .OP_END_FACTORY_REG(ApplyAdagradDA)

/**
*@brief Returns the dimension index in the destination data format given the one in
* the source data format.
*
*@par Inputs:
* x: A tensor of type int32 or int64.
*     A Tensor with each element as a dimension index in source data format.
*     Must be in the range [-4, 4).
*
*@par Attributes:
*@li src_format: An optional string. Defaults to NHWC.
*     source data format.
*@li dst_format: An optional string. Defaults to NCHW.
*     destination data format.
*
*@par Outputs:
* y: A tensor. Has the same type as "x".
*
*/
REG_OP(DataFormatDimMap)
    .INPUT(x, TensorType::IndexNumberType())
    .ATTR(src_format, String, "NHWC")
    .ATTR(dst_format, String, "NCHW")
    .OUTPUT(y, TensorType::IndexNumberType())
    .OP_END_FACTORY_REG(DataFormatDimMap)

/**
* @brief Implements stochastic gradient descent (optionally with momentum).\n
* Nesterov momentum is based on the formula from
* On the importance of initialization and momentum in deep learning.\n

* @par Inputs:
* @li parameters: A mutable tensor of type float16 or float32.\n
* Specifies the iterable of parameters to optimize or dicts defining parameter
* groups.
* @li gradient: A tensor of type float16 or float32.\n
* Specifies the gradient of training step.
* @li learning_rate: A tensor of type float16 or float32.\n
* Specifies the learing_rate of training step.
* @li accum: A tensor of type float16 or float32.
* Specifies the velocity of training step.
* @li momentum: A tensor of type float16 or float32.
* Specifies the momentum factor.
* @li stat: A tensor of type float16 or float32.
* Specifies the status representing the first step or not.

* @par Attributes:
* @li dampening: An optional float, specifying the dampening for momentum.
* Defaults to "0.0".
* @li weight_decay: An optional float, specifying the L2 penalty. Defaults to
* "0.0".
* @li nesterov: An optional bool, specifying whether to enable Nesterov
* momentum. Defaults to "False".

* @par Outputs:
* parameters: A mutable tensor same as input "parameters".

* @see ApplyMomentum()
*/
REG_OP(SGD)
    .INPUT(parameters, TensorType(DT_FLOAT, DT_FLOAT16))
    .INPUT(gradient, TensorType(DT_FLOAT, DT_FLOAT16))
    .INPUT(learning_rate, TensorType(DT_FLOAT, DT_FLOAT16))
    .INPUT(accum, TensorType(DT_FLOAT, DT_FLOAT16))
    .INPUT(momentum, TensorType(DT_FLOAT, DT_FLOAT16))
    .INPUT(stat, TensorType(DT_FLOAT, DT_FLOAT16))
    .OUTPUT(parameters, TensorType(DT_FLOAT, DT_FLOAT16))
    .ATTR(dampening, Float, 0.0)
    .ATTR(weight_decay, Float, 0.0)
    .ATTR(nesterov, Bool, false)
    .OP_END_FACTORY_REG(SGD)

/**
* @brief Updates "var" according to the RMSProp algorithm.\n
*    mean_square = decay * mean_square + (1-decay) * gradient ** 2\n
*    Delta = learning_rate * gradient / sqrt(mean_square + epsilon)\n
*    ms <- rho * ms_{t-1} + (1-rho) * grad * grad\n
*    mom <- momentum * mom_{t-1} + lr * grad / sqrt(ms + epsilon)\n
*    var <- var - mom\n
*
* @attention Constraints:
* @li Note that in dense implementation of this algorithm, "ms" and "mom" will\n
* update even if "grad" is 0, but in this sparse implementation, "ms" and "mom"\n
* will not update in iterations during which "grad" is 0.\n
* @li The input tensors "var", "ms", "mom" and "grad" must have the same shape.
*
* @par Inputs:
* @li var: A mutable tensor. Must be one of the data types defined in\n
* TensorType::NumberType(). Should be from a Variable().
* @li ms: A mutable tensor. Must have the same type as "var". Should be from a
* Variable().
* @li mom: A mutable tensor. Must have the same type as "var". Should be from a
* Variable().
* @li lr: A scalar. Must have the same type as "var".
* @li rho: A scalar. Must have the same type as "var".
* @li momentum: A scalar. Must have the same type as "var".
* @li epsilon: A scalar. Must have the same type as "var".
* @li grad: A tensor, specifying the gradient. Must have the same type as "var".
*
* @par Attributes:
* use_locking: An optional "bool". Defaults to "False". If "True", updating of\n
* the "var", "ms", and "mom" tensors will be protected by a lock; otherwise the\n
* behavior is undefined, but may exhibit less contention.
*
* @par Outputs:
* var: A mutable tensor. Has the same type as input "var".
*/
REG_OP(ApplyRMSProp)
    .INPUT(var, TensorType::NumberType())
    .INPUT(ms, TensorType::NumberType())
    .INPUT(mom, TensorType::NumberType())
    .INPUT(lr, TensorType::NumberType())
    .INPUT(rho, TensorType::NumberType())
    .INPUT(momentum, TensorType::NumberType())
    .INPUT(epsilon, TensorType::NumberType())
    .INPUT(grad, TensorType::NumberType())
    .OUTPUT(var, TensorType::NumberType())
    .ATTR(use_locking, Bool, false)
    .OP_END_FACTORY_REG(ApplyRMSProp)

/**
* @brief Updates "var" according to the RMSProp algorithm, a const input will be
* considered as an attribute.\n
*     mean_square = decay * mean_square + (1-decay) * gradient ** 2\n
*     Delta = learning_rate * gradient / sqrt(mean_square + epsilon)\n
*     ms <- rho * ms_{t-1} + (1-rho) * grad * grad\n
*     mom <- momentum * mom_{t-1} + lr * grad / sqrt(ms + epsilon)\n
*     var <- var - mom
*
* @attention Constraints:
* @li Note that in dense implementation of this algorithm, "ms" and "mom" will\n
* update even if "grad" is 0, but in this sparse implementation, "ms" and "mom"\n
* will not update in iterations during which "grad" is 0.
* @li The input tensors "var", "ms", "mom" and "grad" must have the same shape.
*
* @par Inputs:
* @li var: A mutable tensor. Must be one of the data types defined in\n
* TensorType::NumberType(). Should be from a Variable().
* @li ms: A mutable tensor. Must have the same type as "var". Should be from a
* Variable().
* @li mom: A mutable tensor. Must have the same type as "var". Should be from a
* Variable().
* @li lr: A scalar. Must have the same type as "var".
* @li grad: A tensor, specifying the gradient. Must have the same type as "var".
*
* @par Attributes:
* @li use_locking: An optional "bool". Defaults to "False". If "True", updating\n
* of the "var", "ms", and "mom" tensors will be protected by a lock; otherwise
* the behavior is undefined, but may exhibit less contention.
* @li rho: A scalar. Must have the same type as "var".
* @li momentum: A scalar. Must have the same type as "var".
* @li epsilon: A scalar. Must have the same type as "var".
*
* @par Outputs:
* var: A mutable tensor. Must have the same type as input "var".
*/
REG_OP(ApplyRMSPropD)
    .INPUT(var, TensorType::NumberType())
    .INPUT(ms, TensorType::NumberType())
    .INPUT(mom, TensorType::NumberType())
    .INPUT(lr, TensorType::NumberType())
    .INPUT(grad, TensorType::NumberType())
    .OUTPUT(var, TensorType::NumberType())
    .REQUIRED_ATTR(rho, Float)
    .REQUIRED_ATTR(momentum, Float)
    .REQUIRED_ATTR(epsilon, Float)
    .ATTR(use_locking, Bool, false)
    .OP_END_FACTORY_REG(ApplyRMSPropD)

/**
*@brief Update "var" and "accum" according to FOBOS with Adagrad learning rate.

*@par Inputs:
*Six inputs, including:
* @li var: A mutable Tensor of type TensorType::NumberType().
*    Should be from a Variable().
* @li accum: A mutable Tensor of the same type as "var". Should be from a Variable().
* @li lr: A Tensor of the same type as "var", for the scaling factor. Must be a scalar.
* @li l1: A Tensor of the same type as "var", for L1 regulariation. Must be a scalar.
* @li l2: A Tensor of the same type as "var", for L2 regulariation. Must be a scalar.
* @li grad: A Tensor of the same type as "var", for the gradient.

*@par Attributes:
*use_locking: An optional bool. Defaults to "False". If "True", updating of the "var" and "accum" *tensors will be protected by a lock; otherwise the behavior is undefined, but may exhibit less *contention.

*@par Outputs:
*var: A mutable Tensor. Has the same type as "var".
*/
REG_OP(ApplyProximalAdagrad)
    .INPUT(var, TensorType::NumberType())
    .INPUT(accum, TensorType::NumberType())
    .INPUT(lr, TensorType::NumberType())
    .INPUT(l1, TensorType::NumberType())
    .INPUT(l2, TensorType::NumberType())
    .INPUT(grad, TensorType::NumberType())
    .OUTPUT(var, TensorType::NumberType())
    .ATTR(use_locking, Bool, false)
    .OP_END_FACTORY_REG(ApplyProximalAdagrad)

/**
*@brief Updates entries in 'var' and 'accum' according to the Proximal Adagrad algorithm.\ n
* Compared with op ApplyProximalAdagrad, an additional index tensor is input,
* Only the indices into the first dimensions of "var" and "accum" are updated.

*@par Inputs:
* Seven inputs, including:\n
* @li var: A mutable Tensor.\n
*     TensorType::NumberType(). Should be a Variable Tensor.
* @li accum: A mutable Tensor of the same type as "var".\n
*     Should be a Variable Tensor.
* @li lr: A Tensor of the same type as "var".\n
*     Scaling factor. Must be a scalar.
* @li l1: A Tensor of the same type as "var".\n
*     L1 regulariation. Must be a scalar.
* @li l2: A Tensor of the same type as "var".\n
*     L2 regulariation. Must be a scalar.
* @li grad: A Tensor. Has the same type as "var". \n
*     The gradient.
* @li indices: A vector of indices into the first dimension of "var" and "accum".\n
*     TensorType::IndexNumberType(). 

*@par Attributes:
*use_locking: An optional bool. Defaults to "False".\n
*     If "True", updating of the var and accum tensors will be protected by a lock; \n 
*     If "False", the behavior is undefined, but may exhibit less contention. 

*@par Outputs:
*var: A mutable Tensor. Has the same type as "var".
*/
REG_OP(SparseApplyProximalAdagrad)
    .INPUT(var, TensorType::NumberType())
    .INPUT(accum, TensorType::NumberType())
    .INPUT(lr, TensorType::NumberType())
    .INPUT(l1, TensorType::NumberType())
    .INPUT(l2, TensorType::NumberType())
    .INPUT(grad, TensorType::NumberType())
    .INPUT(indices, TensorType::IndexNumberType())
    .OUTPUT(var, TensorType::NumberType())
    .ATTR(use_locking, Bool, false)
    .OP_END_FACTORY_REG(SparseApplyProximalAdagrad)

/**
*@brief Updates "var" according to the Ftrl-proximal scheme.

*@par Inputs:
*Eight inputs, including:
* @li var: A mutable Tensor. Must be of type TensorType::NumberType().
*     Should be a Variable Tensor.
* @li accum: A mutable Tensor of the same type as "var".
*     Should be a Variable Tensor.
* @li linear: A mutable Tensor of the same type as "var".
*     Should be a Variable Tensor.
* @li grad: A Tensor of the same type as "var", for the gradient.
* @li lr: A Tensor of the same type as "var", for the scaling factor. Must be a scalar.
* @li l1: A Tensor of the same type as "var", for L1 regulariation. Must be a scalar.
* @li l2: A Tensor of the same type as "var", for L2 regulariation. Must be a scalar.
* @li lr_power: A Tensor of the same type as "var", for the scaling factor. Must be a scalar.

*@par Attributes:
*use_locking: An optional bool. Defaults to "False".
*     If "True", updating of the "var" and "accum" tensors will be
*     protected by a lock; otherwise the behavior is undefined,
*     but may exhibit less contention.

*@par Outputs:
*var: A mutable Tensor. Has the same type as "var".
*/
REG_OP(ApplyFtrl)
    .INPUT(var, TensorType::NumberType())
    .INPUT(accum, TensorType::NumberType())
    .INPUT(linear, TensorType::NumberType())
    .INPUT(grad, TensorType::NumberType())
    .INPUT(lr, TensorType::NumberType())
    .INPUT(l1, TensorType::NumberType())
    .INPUT(l2, TensorType::NumberType())
    .INPUT(lr_power, TensorType::NumberType())
    .OUTPUT(var, TensorType::NumberType())
    .ATTR(use_locking, Bool, false)
    .OP_END_FACTORY_REG(ApplyFtrl)

/**
*@brief Update "var" according to the Ftrl-proximal scheme.

*@par Inputs:
*Nine inputs, including:
* @li var: A mutable Tensor. Must be of type TensorType::NumberType().
*     Should be a Variable Tensor.
* @li accum: A mutable Tensor of the same type as "var".
*     Should be a Variable Tensor.
* @li linear: A mutable Tensor of the same type as "var".
*     Should be a Variable Tensor.
* @li grad: A Tensor of the same type as "var", for the gradient.
* @li lr: A Tensor of the same type as "var", for the scaling factor. Must be a scalar.
* @li l1: A Tensor of the same type as "var", for L1 regulariation. Must be a scalar.
* @li l2: A Tensor of the same type as "var", for L2 regulariation. Must be a scalar.
* @li l2_shrinkage: A Tensor of the same type as "var".
* @li lr_power: A Tensor of the same type as "var", for the scaling factor. Must be a scalar.

*@par Attributes:
*use_locking: An optional bool. Defaults to "False".
*     If "True", updating of the "var" and "accum" tensors will be
*     protected by a lock; otherwise the behavior is undefined,
*     but may exhibit less contention.

*@par Outputs:
*var: A mutable Tensor. Has the same type as "var".
*/
REG_OP(ApplyFtrlV2)
    .INPUT(var, TensorType::NumberType())
    .INPUT(accum, TensorType::NumberType())
    .INPUT(linear, TensorType::NumberType())
    .INPUT(grad, TensorType::NumberType())
    .INPUT(lr, TensorType::NumberType())
    .INPUT(l1, TensorType::NumberType())
    .INPUT(l2, TensorType::NumberType())
    .INPUT(l2_shrinkage, TensorType::NumberType())
    .INPUT(lr_power, TensorType::NumberType())
    .OUTPUT(var, TensorType::NumberType())
    .ATTR(use_locking, Bool, false)
    .OP_END_FACTORY_REG(ApplyFtrlV2)

/**
*@brief Updates "var" according to the Adam algorithm.\n
*  lr_t <- text{learning\_rate} * sqrt{1 - beta_2^t} / (1 - beta_1^t)\n
*  m_t <- beta_1 * m_{t-1} + (1 - beta_1) * g\n
*  v_t <- max(beta2 * v{t-1}, abs(g))\n
*  variable <- variable - lr_t * m_t / (sqrt{v_t} + epsilon)
*
*@attention Constraints:\n
*  *The input tensors must have the same shape.*
*
*@par Inputs:
*@li var: A mutable Tensor of the type TensorType::NumberType().
*     Should be from a Variable().
*@li m: A mutable Tensor of the same type as "var".
*     Should be from a Variable().
*@li v: A mutable Tensor of the same type as "var".
*     Should be from a Variable().
*@li beta1_power: A scalar of the same type as "var".
*@li beta2_power: A scalar of the same type as "var".
*@li lr: learning_rate. A scalar of the same type as "var".
*@li beta1: A scalar of the same type as "var".
*@li beta2: A scalar of the same type as "var".
*@li epsilon: A scalar of the same type as "var".
*@li grad: A Tensor of the same type as "var", for the gradient.
*
*@par Attributes:\n
*@li use_locking: An optional bool. Defaults to "False".
*     If "True", updating of the "var", m", and "v" tensors will be protected
*     by a lock; otherwise the behavior is undefined, but may exhibit less
*     contention.
*@li use_nesterov: An optional bool. Defaults to "False".
      If "True", uses the nesterov update.
*
*@par Outputs:
* var: A mutable Tensor. Has the same type as intput "var".
*/
REG_OP(ApplyAdam)
    .INPUT(var, TensorType::NumberType())
    .INPUT(m, TensorType::NumberType())
    .INPUT(v, TensorType::NumberType())
    .INPUT(beta1_power, TensorType::NumberType())
    .INPUT(beta2_power, TensorType::NumberType())
    .INPUT(lr, TensorType::NumberType())
    .INPUT(beta1, TensorType::NumberType())
    .INPUT(beta2, TensorType::NumberType())
    .INPUT(epsilon, TensorType::NumberType())
    .INPUT(grad, TensorType::NumberType())
    .OUTPUT(var, TensorType::NumberType())
    .ATTR(use_locking, Bool, false)
    .ATTR(use_nesterov, Bool, false)
    .OP_END_FACTORY_REG(ApplyAdam)

/**
*@brief Updates "var" according to the proximal adadelta scheme.

*@par Inputs:
*Seven inputs, including:
* @li var: A mutable Tensor of type TensorType::NumberType().
*     Should be a Variable Tensor.
* @li accum: A mutable Tensor of the same type as "var".
*     Should be a Variable Tensor.
* @li accum_update: A mutable Tensor of the same type as "var".
*     Should be a Variable Tensor.
* @li lr: A scalar of the same type as "var", for the scaling factor.
* @li rho: A scalar of the same type as "var", for the decay factor.
* @li epsilon: A scalar of the same type as "var", for the constant factor.
* @li grad: A Tensor of the same type as "var", for the gradient.

*@par Attributes:
*use_locking: An optional bool. Defaults to "False".
*     If "True", updating of the "var", "accum" and "accum_update" tensors will be
*     protected by a lock; otherwise the behavior is undefined,
*     but may exhibit less contention.

*@par Outputs:
*var: A mutable Tensor. Has the same type as "var".
*/
REG_OP(ApplyAdadelta)
    .INPUT(var, TensorType::NumberType())
    .INPUT(accum, TensorType::NumberType())
    .INPUT(accum_update, TensorType::NumberType())
    .INPUT(lr, TensorType::NumberType())
    .INPUT(rho, TensorType::NumberType())
    .INPUT(epsilon, TensorType::NumberType())
    .INPUT(grad, TensorType::NumberType())
    .OUTPUT(var, TensorType::NumberType())
    .ATTR(use_locking, Bool, false)
    .OP_END_FACTORY_REG(ApplyAdadelta)

REG_OP(FusedMulApplyMomentum)
  .INPUT(var, TensorType::NumberType())
  .INPUT(accum, TensorType::NumberType())
  .INPUT(lr, TensorType::NumberType())
  .INPUT(x1, TensorType::NumberType())
  .INPUT(momentum, TensorType::NumberType())
  .INPUT(x2, TensorType::NumberType())
  .OUTPUT(var, TensorType::NumberType())
  .ATTR(use_nesterov, Bool, false)
  .ATTR(use_locking, Bool, false)
  .OP_END_FACTORY_REG(FusedMulApplyMomentum)

REG_OP(FusedMulApplyMomentumExtern)
  .INPUT(var, TensorType::NumberType())
  .INPUT(accum, TensorType::NumberType())
  .INPUT(lr, TensorType::NumberType())
  .INPUT(x1, TensorType::NumberType())
  .INPUT(momentum, TensorType::NumberType())
  .INPUT(x2, TensorType::NumberType())
  .INPUT(var_copy, TensorType::NumberType())
  .OUTPUT(var, TensorType::NumberType())
  .OUTPUT(var_copy, TensorType::NumberType())
  .ATTR(use_nesterov, Bool, false)
  .ATTR(use_locking, Bool, false)
  .OP_END_FACTORY_REG(FusedMulApplyMomentumExtern)

REG_OP(LarsV2)
    .INPUT(w, TensorType(DT_FLOAT))
    .INPUT(g, TensorType(DT_FLOAT))
    .INPUT(weight_decay, TensorType(DT_FLOAT))
    .INPUT(learning_rate, TensorType(DT_FLOAT))
    .OUTPUT(g_new, TensorType(DT_FLOAT))
    .ATTR(hyperpara, Float, 0.001)
    .ATTR(epsilon, Float, 0.00001)
    .ATTR(use_clip, Bool, false)
    .OP_END_FACTORY_REG(LarsV2)

REG_OP(LarsV2Update)
    .INPUT(w, TensorType(DT_FLOAT))
    .INPUT(g, TensorType(DT_FLOAT))
    .INPUT(w_square_sum, TensorType(DT_FLOAT))
    .INPUT(g_square_sum, TensorType(DT_FLOAT))
    .INPUT(weight_decay, TensorType(DT_FLOAT))
    .INPUT(learning_rate, TensorType(DT_FLOAT))
    .OUTPUT(g_new, TensorType(DT_FLOAT))
    .ATTR(hyperpara, Float, 0.001)
    .ATTR(epsilon, Float, 0.00001)
    .ATTR(use_clip, Bool, false)
    .OP_END_FACTORY_REG(LarsV2Update)

}  // namespace ge

#endif // GE_OP_TRAINING_OPS_H
