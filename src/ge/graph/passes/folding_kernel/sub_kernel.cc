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

#include "graph/passes/folding_kernel/sub_kernel.h"

#include <memory>

#include "common/debug/log.h"
#include "common/fp16_t.h"
#include "common/op/ge_op_utils.h"
#include "graph/common/bcast.h"
#include "graph/utils/type_utils.h"
#include "inc/kernel_factory.h"

namespace ge {
namespace {
const size_t kSubFirstInput = 0;
const size_t kSubSecondInput = 1;
const size_t kSubFirstOutput = 0;
const size_t kSubOutputSize = 1;
const size_t kSubInputSize = 2;

#define DEFINE_FUNC_BY_TYPE(TYPE)                                                                          \
  std::function<TYPE(TYPE const &, TYPE const &)> func_##TYPE = [](TYPE const &a, TYPE const &b) -> TYPE { \
    return a - b;                                                                                          \
  };

#define SET_BCAST_COMPUTE_CASE(DTYPE, TYPE)                      \
  case DTYPE:                                                    \
    ret = bcast.BCastCompute(input, y_data_##TYPE, func_##TYPE); \
    break;

#define SET_OUTPUT(DTYPE, TYPE)                                                                                  \
  case DTYPE:                                                                                                    \
    (void)output_ptr->SetData(reinterpret_cast<uint8_t *>(y_data_##TYPE.data()), y_data_##TYPE.size() * length); \
    break;

DEFINE_FUNC_BY_TYPE(int8_t)
DEFINE_FUNC_BY_TYPE(int16_t)
DEFINE_FUNC_BY_TYPE(int32_t)
DEFINE_FUNC_BY_TYPE(int64_t)
DEFINE_FUNC_BY_TYPE(uint8_t)
DEFINE_FUNC_BY_TYPE(uint16_t)
DEFINE_FUNC_BY_TYPE(uint32_t)
DEFINE_FUNC_BY_TYPE(uint64_t)
DEFINE_FUNC_BY_TYPE(float)
DEFINE_FUNC_BY_TYPE(double)
}  // namespace

Status SubKernel::Compute(const ge::OpDescPtr op_desc_ptr, const std::vector<ge::ConstGeTensorPtr> &input,
                          vector<ge::GeTensorPtr> &v_output) {
  GE_CHECK_NOTNULL(op_desc_ptr);
  // check how many inputs
  if ((input.size() != kSubInputSize) || (op_desc_ptr->GetOutputsSize() != kSubOutputSize)) {
    GELOGW("The number of input for sub must be %zu.", kSubInputSize);
    return PARAM_INVALID;
  }

  GE_CHECK_NOTNULL(input[kSubFirstInput]);
  GE_CHECK_NOTNULL(input[kSubSecondInput]);
  ConstGeTensorPtr weight0 = input[kSubFirstInput];
  ConstGeTensorPtr weight1 = input[kSubSecondInput];

  //  to broadcast
  std::vector<int8_t> y_data_int8_t;
  std::vector<int16_t> y_data_int16_t;
  std::vector<int32_t> y_data_int32_t;
  std::vector<int64_t> y_data_int64_t;
  std::vector<uint8_t> y_data_uint8_t;
  std::vector<uint16_t> y_data_uint16_t;
  std::vector<uint32_t> y_data_uint32_t;
  std::vector<uint64_t> y_data_uint64_t;
  std::vector<fp16_t> y_data_fp16_t;
  std::vector<float> y_data_float;
  std::vector<double> y_data_double;

  Status ret;
  DataType data_type = input[kSubFirstInput]->GetTensorDesc().GetDataType();
  BCast bcast;
  switch (data_type) {
    SET_BCAST_COMPUTE_CASE(DT_INT8, int8_t)
    SET_BCAST_COMPUTE_CASE(DT_INT16, int16_t)
    SET_BCAST_COMPUTE_CASE(DT_INT32, int32_t)
    SET_BCAST_COMPUTE_CASE(DT_INT64, int64_t)
    SET_BCAST_COMPUTE_CASE(DT_UINT8, uint8_t)
    SET_BCAST_COMPUTE_CASE(DT_UINT16, uint16_t)
    SET_BCAST_COMPUTE_CASE(DT_UINT32, uint32_t)
    SET_BCAST_COMPUTE_CASE(DT_UINT64, uint64_t)
    SET_BCAST_COMPUTE_CASE(DT_FLOAT, float)
    SET_BCAST_COMPUTE_CASE(DT_DOUBLE, double)
    default:
      ret = NOT_CHANGED;
      break;
  }

  if (ret != SUCCESS) {
    GELOGW("BCastCompute fail, data_type:%s, ret:%s", TypeUtils::DataTypeToSerialString(data_type).c_str(),
           GET_ERRORNO_STR(ret).c_str());
    return NOT_CHANGED;
  }

  uint32_t length = 1;
  if (!TypeUtils::GetDataTypeLength(data_type, length)) {
    GELOGW("Can't GetDataTypeLength of data_type: %s", TypeUtils::DataTypeToSerialString(data_type).c_str());
    return NOT_CHANGED;
  }

  auto output_tensor_desc = op_desc_ptr->GetOutputDesc(kSubFirstOutput);
  GeTensorPtr output_ptr = MakeShared<GeTensor>(output_tensor_desc);
  if (output_ptr == nullptr) {
    GELOGE(MEMALLOC_FAILED, "make_shared ge::GeTensor failed, node name %s.", op_desc_ptr->GetName().c_str());
    return NOT_CHANGED;
  }

  output_ptr->MutableTensorDesc().SetShape(GeShape(bcast.GetOutputShape()));
  // only return GRAPH_SUCCESS here
  switch (data_type) {
    SET_OUTPUT(DT_INT8, int8_t)
    SET_OUTPUT(DT_INT16, int16_t)
    SET_OUTPUT(DT_INT32, int32_t)
    SET_OUTPUT(DT_INT64, int64_t)
    SET_OUTPUT(DT_UINT8, uint8_t)
    SET_OUTPUT(DT_UINT16, uint16_t)
    SET_OUTPUT(DT_UINT32, uint32_t)
    SET_OUTPUT(DT_UINT64, uint64_t)
    SET_OUTPUT(DT_FLOAT16, fp16_t)
    SET_OUTPUT(DT_FLOAT, float)
    SET_OUTPUT(DT_DOUBLE, double)
    default:
      break;
  }
  output_ptr->MutableTensorDesc().SetDataType(data_type);
  v_output.push_back(output_ptr);

  return SUCCESS;
}

REGISTER_KERNEL(SUB, SubKernel);
}  // namespace ge
