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

#ifndef GE_OP_NN_POOLING_OPS_H
#define GE_OP_NN_POOLING_OPS_H

#include "../graph/operator_reg.h"
/**
*@brief Performs pooling on the input.
*@par Inputs:
*@li x: An NCHW tensor of type float16.
*@par Attributes:
*@li mode: An optional int32, specifying the pooling algorithm, either "1" (max pooling) or "0" (avg pooling). Defaults to "0".
*@li global_pooling: An optional bool. Defaults to "false".
*@li window: Optional, including: \n
*window[0]: An optional int32, specifying the window size along in the H dimension. The value range is [1, 32768]. Defaults to "1". \n
*window[1]: An optional int32, specifying the window size along in the W dimension. The value range is [1, 32768]. Defaults to "1". \n
*@li stride: Optional, including: \n
*stride[0]: An optional int32, specifying the stride along in the H dimension. The value range is [1, 63]. Defaults to "1". \n
*stride[1]: An optional int32, specifying the stride along in the W dimension. The value range is [1, 63]. Defaults to "1". \n
*@li pad: Optional, including: \n
*pad[0]: An optional int32, specifying the up padding. Defaults to "0". \n
*pad[1]: An optional int32, specifying the bottom padding. Defaults to "0". \n
*pad[2]: An optional int32, specifying the left padding. Defaults to "0". \n
*pad[3]: An optional int32, specifying the right padding. Defaults to "0". \n
*@li ceil_mode: An optional int32, either "0" (ceil mode) or "1" (floor mode). Defaults to "0".
*@par Outputs:
*y: An NCHW tensor of type float16.
*@attention Constraints:\n
*@li window[0] * window[1] < 256;
*/
namespace ge {
REG_OP(Pooling)
    .INPUT(x, TensorType({DT_FLOAT16}))
    .OUTPUT(y, TensorType({DT_FLOAT16}))
    .ATTR(mode, Int, 0)                 // 0:max pooling or 1:avg pooling
    .ATTR(global_pooling, Bool, false)
    .ATTR(window, ListInt, {1,1})       // kernel size
    .ATTR(stride, ListInt, {1,1})       // stride size
    .ATTR(pad, ListInt, {0,0,0,0})      // pad size
    .ATTR(ceil_mode, Int, 0)
    .OP_END_FACTORY_REG(Pooling)

/**
*@brief Performs average pooling on the input.

*@par Inputs:
*x: A tensor of type float16.

*@par Attributes:
*@li ksize: A required list of 4 ints, specifying the size (N, C, H, and W) of the sliding window, where N = C = 1, and H and W are positive integers within the range [1, 32768].
*@li strides: A required list of 4 ints, specifying the stride of the sliding window. The strides of the N and C dimensions are 1. The strides of the H and W dimensions are positive integers within the range [1, 63].
*@li padding: A required string, specifying the padding algorithm, either "VALID" or "SAME". With "SAME" means that the outputs will have the same spatial dimensions as its inputs. With "VALID" means no padding.
*@li data_format: An optional string, specifying the data format of "ksize" and "strides", either "NCHW", "NC1HWC0", or "NHWC" (default).

*@par Outputs:
*y: The average pooled output tensor.

*@attention Constraints:\n
*@li Only single input and single output are supported.
*@li Global pooling is supported.
*@li "ksize_H" and "ksize_W" are positive integers within the range [1, 32768]. ksize_H * ksize_W < 256
*@li Due to instruction restrictions, the values of "strides_h" and "strides_w" are positive integers within the range [1, 63].
*/
REG_OP(AvgPool)
    .INPUT(x, TensorType({DT_FLOAT16, DT_FLOAT32, DT_DOUBLE}))
    .OUTPUT(y, TensorType({DT_FLOAT16, DT_FLOAT32, DT_DOUBLE}))
    .REQUIRED_ATTR(ksize, ListInt)
    .REQUIRED_ATTR(strides, ListInt)
    .REQUIRED_ATTR(padding, String)
    .ATTR(data_format, String, "NHWC")
    .OP_END_FACTORY_REG(AvgPool)

REG_OP(MaxPoolExt2)
    .INPUT(x, TensorType({DT_FLOAT16, DT_FLOAT32, DT_DOUBLE, DT_INT8,
                          DT_INT16, DT_INT32, DT_INT64, DT_UINT8,
                          DT_UINT16, DT_QINT8}))
    .OUTPUT(y, TensorType({DT_FLOAT16, DT_FLOAT32, DT_DOUBLE, DT_INT8,
                           DT_INT16, DT_INT32, DT_INT64, DT_UINT8,
                           DT_UINT16, DT_QINT8}))
    .REQUIRED_ATTR(ksize, ListInt)
    .REQUIRED_ATTR(strides, ListInt)
    .REQUIRED_ATTR(padding, String)
    .ATTR(data_format, String, "NHWC")
    .OP_END_FACTORY_REG(MaxPoolExt2)

REG_OP(MaxPool)
    .INPUT(x, TensorType({DT_FLOAT16, DT_FLOAT32, DT_DOUBLE, DT_INT8,
                          DT_INT16, DT_INT32, DT_INT64, DT_UINT8,
                          DT_UINT16, DT_QINT8}))
    .OUTPUT(y, TensorType({DT_FLOAT16, DT_FLOAT32, DT_DOUBLE, DT_INT8,
                           DT_INT16, DT_INT32, DT_INT64, DT_UINT8, DT_UINT16, DT_QINT8}))
    .REQUIRED_ATTR(ksize, ListInt)
    .REQUIRED_ATTR(strides, ListInt)
    .REQUIRED_ATTR(padding, String)
    .ATTR(data_format, String, "NHWC")
    .OP_END_FACTORY_REG(MaxPool)

/**
* @brief Computes gradients of the maxpooling function.

* @par Inputs:
* @li x1: A mutable NC1HWC0 tensor of type RealNumberType.
* @li x2: A mutable NC1HWC0 tensor of type RealNumberTypex.
* @li grad: A mutable NC1HWC0 tensor of type RealNumberType.

* @par Attributes:
* @li ksize: A tuple or list, specifying the size of the window for each
* dimension of the input tensor.
* @li strides: A tuple or list, specifying the stride of the sliding window for
* each dimension of the input tensor.
* @li padding: A string, specifying the type of padding algorithm to use.

* @par Outputs:
* y: A mutable tensor. Has the same shape and type as "x1.

* @attention Constraints:
* @li Computing gradients of global pooling is not supported, which means
* "ksize < x1".
* @li "ksiez" is in the range [1, 255]. "strides" is in the range [1, 63]
*/
REG_OP(MaxPoolGrad)
    .INPUT(x1, TensorType::RealNumberType())
    .INPUT(x2, TensorType::RealNumberType())
    .INPUT(grad, TensorType::RealNumberType())
    .OUTPUT(y, TensorType::RealNumberType())
    .REQUIRED_ATTR(ksize, ListInt)
    .REQUIRED_ATTR(strides, ListInt)
    .REQUIRED_ATTR(padding, String)
    .OP_END_FACTORY_REG(MaxPoolGrad)

/**
* @brief Computes second-order gradients of the maxpooling function.

* @par Inputs:
* @li x1: Original forward input tensor of type float16
* @li x2: Original forward output tensor of type float16
* @li grad: Gradient tensor of type float16

* @par Attributes:
* @li ksize: A required list, specifying the size of the sliding window.
* @li strides: A required list, specifying the stride of the sliding window.
* @li padding: window sliding mode. Either SAME or VALID.
* @li data_format: Format of the original input, either NCHW or NHWC. Defaults
* to NHWC.

* @attention Constraints:
* @li Only the cloud platform is supported.
* @li "x1" and "grads" must have the same shape.
* @li "x2" and "y" must have the same shape. Otherwise, an error is reported.
* @li "x1", "x2", "grads", and "y" must be 5D tensors.

* @par Outputs:
* @li y: Result tensor of type float16
*/
REG_OP(MaxPoolGradGrad)
    .INPUT(x1, TensorType::RealNumberType())
    .INPUT(x2, TensorType::RealNumberType())
    .INPUT(grad, TensorType::RealNumberType())
    .OUTPUT(y, TensorType::RealNumberType())
    .REQUIRED_ATTR(ksize, ListInt)
    .REQUIRED_ATTR(strides, ListInt)
    .REQUIRED_ATTR(padding, String)
    .ATTR(data_format, String, "NHWC")
    .OP_END_FACTORY_REG(MaxPoolGradGrad)

/**
*@brief Performs max_pool_ext2 on the input.

*@par Inputs:
* Two inputs:
*@li x: An NC1HWC0 Tensor of type float16.
*@li strides: A required type of int32 values, specifying the stride of the sliding window for each dimension of the input tensor. No default value.
*@li ksize: A required type of int32 values, specifying the size of the window for each dimension of the input tensor. No default value.


*@par Attributes:
*@li padding: A required string. No default value.
*@li data_format: An optional string. Defaults to "NC1HWC0".

*@par Outputs:
*y: A Tensor. Has the same type and format as input "x".

*@attention Constraints:
*@li "ksize" is a list that has length 4: ksize[0] = 1 or ksize[3] = 1, ksize[1] * ksize[2] <= 255.
*@li "stride is a list that has length 4: strides[0] = 1 or strides[3] = 1, strides[1] <= 63, strides[0] >= 1, strides[2] <= 63, strides[2] >= 1.
*@li "padding" is either "SAME" or "VALID".
*/
REG_OP(MaxPoolV2)
    .INPUT(x, TensorType({DT_FLOAT16}))
    .INPUT(ksize, TensorType({DT_INT32}))
    .INPUT(strides, TensorType({DT_INT32}))
    .OUTPUT(y, TensorType({DT_FLOAT16}))
    .REQUIRED_ATTR(padding, String)
    .ATTR(data_format, String, "NHWC")
    .OP_END_FACTORY_REG(MaxPoolV2)

REG_OP(MaxPoolWithArgmax)
    .INPUT(x, TensorType::RealNumberType())
    .OUTPUT(y, TensorType::RealNumberType())
    .OUTPUT(argmax, TensorType::IndexNumberType())
    .REQUIRED_ATTR(ksize, ListInt)
    .REQUIRED_ATTR(strides, ListInt)
    .REQUIRED_ATTR(padding, String)
    .ATTR(Targmax, Int, 7)
    .OP_END_FACTORY_REG(MaxPoolWithArgmax)

REG_OP(MaxPoolGradWithArgmax)
    .INPUT(x, TensorType::RealNumberType())
    .INPUT(grad, TensorType::RealNumberType())
    .INPUT(argmax, TensorType::IndexNumberType())
    .OUTPUT(y, TensorType::RealNumberType())
    .REQUIRED_ATTR(ksize, ListInt)
    .REQUIRED_ATTR(strides, ListInt)
    .REQUIRED_ATTR(padding, String)
    .OP_END_FACTORY_REG(MaxPoolGradWithArgmax)

/**
* @brief Computes second-order gradients of the maxpooling function.

* @par Inputs:
* @li x:Original forward input tensor of type float16
* @li grad:Gradient tensor of type float16
* @li argmax:An tensor of type uint16
* @par Attributes:
* @li ksize: A required list, specifying the size of the sliding window.
* @li strides: A required list, specifying the stride of the sliding window.
* @li padding: window sliding mode. Either SAME or VALID.
* @par Outputs:
* @li y:Result tensor of type float16

* @attention Constraints:
* @li Only the cloud platform is supported.
* @li "x1" and "grads" must have the same shape.
* @li length of the shape of x, grads, argmax, y must be 5.
* @li shape of argmax must be (fmap_n, fmap_c1, kernel_h * kernel_w,
* (shape_max_pool[2] * shape_max_pool[3] + 15) // 16 * 16, 1),
* or (fmap_n, fmap_c1, kernel_h * kernel_w,
* (shape_max_pool[2] * shape_max_pool[3] + 31) // 16, 16), else failed.
*/
REG_OP(MaxPoolGradGradWithArgmax)
    .INPUT(x, TensorType::RealNumberType())
    .INPUT(grad, TensorType::RealNumberType())
    .INPUT(argmax, TensorType::IndexNumberType())
    .OUTPUT(y, TensorType::RealNumberType())
    .REQUIRED_ATTR(ksize, ListInt)
    .REQUIRED_ATTR(strides, ListInt)
    .REQUIRED_ATTR(padding, String)
    .OP_END_FACTORY_REG(MaxPoolGradGradWithArgmax)

/**
* @brief Computes avgpoograd function.

* @par Inputs:
* @li orig_input_shape: An NHWC tensor of type int32.
* @li input_grad: An NHWC tensor of type float16, float32, or double.

* @par Attributes:
* @li ksize: A tuple or list, specifying the size of the window for each
* dimension of the input tensor.
* @li strides: A tuple or list, specifying the stride of the sliding window for\n
* each dimension of the input tensor.
* @li padding: A string, specifying the type of the padding algorithm to use.
* @li data_format: A string. Defaults to "NHWC".

* @par Outputs:
* @out_grad: A mutable tensor with the same shape and type as "orig_input".
*/
REG_OP(AvgPoolGrad)
    .INPUT(orig_input_shape, TensorType({DT_INT32}))
    .INPUT(input_grad, TensorType({DT_FLOAT16, DT_FLOAT32, DT_DOUBLE}))
    .OUTPUT(out_grad, TensorType({DT_FLOAT16, DT_FLOAT32, DT_DOUBLE}))
    .REQUIRED_ATTR(ksize, ListInt)
    .REQUIRED_ATTR(strides, ListInt)
    .REQUIRED_ATTR(padding, String)
    .ATTR(data_format, String, "NHWC")
    .OP_END_FACTORY_REG(AvgPoolGrad)

/**
* @brief Computes gradients of average pooling function.

* @par Inputs:
* @input_grad: An NHWC tensor of type float16, float32, or double.

* @par Attributes:
* @li orig_input_shape: Original input dimensions.
* @li ksize: A tuple or list, specifying the size of the window for each
* dimension of the input tensor.
* @li strides: A tuple or list, specifying the stride of the sliding window for\n
* each dimension of the input tensor.
* @li padding: A string, specifying the type of the padding algorithm to use.
* @li data_format: A string. Defaults to "NHWC".

* @par Outputs:
* @out_grad: A mutable tensor with the same shape and type as "orig_input".
*/
REG_OP(AvgPoolGradD)
    .INPUT(input_grad, TensorType({DT_FLOAT16, DT_FLOAT32, DT_DOUBLE}))
    .OUTPUT(out_grad, TensorType({DT_FLOAT16, DT_FLOAT32, DT_DOUBLE}))
    .REQUIRED_ATTR(orig_input_shape, ListInt)
    .REQUIRED_ATTR(ksize, ListInt)
    .REQUIRED_ATTR(strides, ListInt)
    .REQUIRED_ATTR(padding, String)
    .ATTR(data_format, String, "NHWC")

    .OP_END_FACTORY_REG(AvgPoolGradD)

REG_OP(MaxPoolWithArgmaxCCE)
    .INPUT(x, TensorType::ALL())
    .OUTPUT(y, TensorType::ALL())
    .OUTPUT(argmax, TensorType::ALL())
    .ATTR(mode, Int, 0)
    .ATTR(pad_mode, Int, 0)
    .ATTR(window, ListInt, {1,1})
    .ATTR(stride, ListInt, {1,1})
    .ATTR(pad, ListInt, {0,0,0,0})
    .ATTR(ceil_mode, Int, 0)
    .ATTR(data_mode, Int, 1)
    .ATTR(nan_opt, Int, 0)
    .OP_END_FACTORY_REG(MaxPoolWithArgmaxCCE)

REG_OP(MaxPoolGradWithArgmaxCCE)
   .INPUT(x, TensorType::ALL())
   .INPUT(grad,TensorType::ALL())
   .INPUT(arg,TensorType::ALL())
   .OUTPUT(output,TensorType::ALL())
   .ATTR(mode, Int, 0)
   .ATTR(max_pool_grad_output_shape, ListInt, {0,0,0,0})
   .ATTR(pad_mode, Int, 0)
   .ATTR(window, ListInt, {1,1})
   .ATTR(stride, ListInt, {1,1})
   .ATTR(pad, ListInt, {0,0,0,0})
   .ATTR(ceil_mode, Int, 0)
   .ATTR(data_mode, Int, 1)
   .ATTR(nan_opt, Int, 0)
   .OP_END_FACTORY_REG(MaxPoolGradWithArgmaxCCE)

REG_OP(Upsample)
   .INPUT(x, TensorType({DT_FLOAT16, DT_FLOAT, DT_INT8, DT_UINT8}))
   .OUTPUT(y, TensorType({DT_FLOAT16, DT_FLOAT, DT_INT8, DT_UINT8}))
   .ATTR(scale, Float, 1)
   .ATTR(stride, Int, 2)
   .OP_END_FACTORY_REG(Upsample)
}  // namespace ge

#endif  // GE_OP_NN_POOLING_OPS_H
