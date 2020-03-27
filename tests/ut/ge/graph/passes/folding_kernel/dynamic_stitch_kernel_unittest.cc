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

#include <gtest/gtest.h>

#define protected public
#define private public
#include "graph/passes/folding_kernel/dynamic_stitch_kernel.h"

#include "common/debug/log.h"
#include "common/debug/memory_dumper.h"
#include "common/fp16_t.h"
#include "common/ge_inner_error_codes.h"
#include "common/op/attr_value_util.h"
#include "common/types.h"
#include "graph/debug/ge_attr_define.h"
#include "graph/passes/folding_kernel/kernel_utils.h"
#include "graph/types.h"
#include "graph/utils/attr_utils.h"
#include "graph/utils/graph_utils.h"
#include "graph/utils/op_desc_utils.h"
#include "graph/utils/tensor_utils.h"
#include "inc/kernel_factory.h"
#undef protected
#undef private

using namespace domi;
using namespace testing;
using namespace ge;

class UTEST_graph_passes_folding_kernel_dynamic_stitch_kernel : public testing::Test {
 protected:
  void SetUp() {}

  void TearDown() {}
};

TEST_F(UTEST_graph_passes_folding_kernel_dynamic_stitch_kernel, indice_float_success) {
  OpDescPtr op_desc_ptr = make_shared<OpDesc>("dynamicstitch", "DynamicStitch");
  AttrUtils::SetInt(op_desc_ptr, "DynamicStitchN_", (int64_t)2);
  vector<bool> is_input_const_vec = {true, true, true, true};
  op_desc_ptr->SetIsInputConst(is_input_const_vec);

  vector<int64_t> dims_vec_0 = {4};
  vector<int32_t> data_vec_0 = {0, 1, 2, 3};
  GeTensorDesc tensor_desc_0(GeShape(dims_vec_0), FORMAT_NCHW, DT_INT32);
  ConstGeTensorPtr tensor_0 =
      make_shared<GeTensor>(tensor_desc_0, (uint8_t *)data_vec_0.data(), data_vec_0.size() * sizeof(int32_t));

  vector<int64_t> dims_vec_1 = {2};
  vector<int32_t> data_vec_1 = {5, 4};
  GeTensorDesc tensor_desc_1(GeShape(dims_vec_1), FORMAT_NCHW, DT_INT32);
  ConstGeTensorPtr tensor_1 =
      make_shared<GeTensor>(tensor_desc_1, (uint8_t *)data_vec_1.data(), data_vec_1.size() * sizeof(int32_t));

  vector<int64_t> dims_vec_2 = {4};
  vector<float> data_vec_2 = {4, 3, 5, 6};
  GeTensorDesc tensor_desc_2(GeShape(dims_vec_2), FORMAT_NCHW, DT_FLOAT);
  ConstGeTensorPtr tensor_2 =
      make_shared<GeTensor>(tensor_desc_2, (uint8_t *)data_vec_2.data(), data_vec_2.size() * sizeof(float));

  vector<int64_t> dims_vec_3 = {2};
  vector<float> data_vec_3 = {7, 8};
  GeTensorDesc tensor_desc_3(GeShape(dims_vec_3), FORMAT_NCHW, DT_FLOAT);
  ConstGeTensorPtr tensor_3 =
      make_shared<GeTensor>(tensor_desc_3, (uint8_t *)data_vec_3.data(), data_vec_3.size() * sizeof(float));

  op_desc_ptr->AddInputDesc(tensor_desc_0);
  op_desc_ptr->AddInputDesc(tensor_desc_1);
  op_desc_ptr->AddInputDesc(tensor_desc_2);
  op_desc_ptr->AddInputDesc(tensor_desc_3);

  vector<ConstGeTensorPtr> input = {tensor_0, tensor_1, tensor_2, tensor_3};
  vector<GeTensorPtr> v_output;

  shared_ptr<Kernel> kernel = KernelFactory::Instance().Create(DYNAMICSTITCH);
  domi::Status status = kernel->Compute(op_desc_ptr, input, v_output);
  EXPECT_EQ(status, ge::SUCCESS);
  float *output_data = const_cast<float *>(reinterpret_cast<const float *>(v_output[0]->GetData().data()));
  EXPECT_FLOAT_EQ(output_data[0], 4);
  EXPECT_FLOAT_EQ(output_data[1], 3);
  EXPECT_FLOAT_EQ(output_data[2], 5);
  EXPECT_FLOAT_EQ(output_data[3], 6);
  EXPECT_FLOAT_EQ(output_data[4], 8);
  EXPECT_FLOAT_EQ(output_data[5], 7);
}

TEST_F(UTEST_graph_passes_folding_kernel_dynamic_stitch_kernel, scaler_indice_double_success) {
  OpDescPtr op_desc_ptr = make_shared<OpDesc>("dynamicstitch", "DynamicStitch");
  AttrUtils::SetInt(op_desc_ptr, "DynamicStitchN_", (int64_t)2);
  vector<bool> is_input_const_vec = {true, true, true, true};
  op_desc_ptr->SetIsInputConst(is_input_const_vec);

  vector<int64_t> dims_vec_0 = {4};
  vector<int32_t> data_vec_0 = {0, 1, 2, 3};
  GeTensorDesc tensor_desc_0(GeShape(dims_vec_0), FORMAT_NCHW, DT_INT32);
  ConstGeTensorPtr tensor_0 =
      make_shared<GeTensor>(tensor_desc_0, (uint8_t *)data_vec_0.data(), data_vec_0.size() * sizeof(int32_t));

  vector<int64_t> dims_vec_1 = {};
  vector<int32_t> data_vec_1 = {4};
  GeTensorDesc tensor_desc_1(GeShape(dims_vec_1), FORMAT_NCHW, DT_INT32);
  ConstGeTensorPtr tensor_1 =
      make_shared<GeTensor>(tensor_desc_1, (uint8_t *)data_vec_1.data(), data_vec_1.size() * sizeof(int32_t));

  vector<int64_t> dims_vec_2 = {4};
  vector<double> data_vec_2 = {4, 3, 5, 6};
  GeTensorDesc tensor_desc_2(GeShape(dims_vec_2), FORMAT_NCHW, DT_DOUBLE);
  ConstGeTensorPtr tensor_2 =
      make_shared<GeTensor>(tensor_desc_2, (uint8_t *)data_vec_2.data(), data_vec_2.size() * sizeof(double));

  vector<int64_t> dims_vec_3 = {};
  vector<double> data_vec_3 = {1};
  GeTensorDesc tensor_desc_3(GeShape(dims_vec_3), FORMAT_NCHW, DT_DOUBLE);
  ConstGeTensorPtr tensor_3 =
      make_shared<GeTensor>(tensor_desc_3, (uint8_t *)data_vec_3.data(), data_vec_3.size() * sizeof(double));

  op_desc_ptr->AddInputDesc(tensor_desc_0);
  op_desc_ptr->AddInputDesc(tensor_desc_1);
  op_desc_ptr->AddInputDesc(tensor_desc_2);
  op_desc_ptr->AddInputDesc(tensor_desc_3);

  vector<ConstGeTensorPtr> input = {tensor_0, tensor_1, tensor_2, tensor_3};
  vector<GeTensorPtr> v_output;

  shared_ptr<Kernel> kernel = KernelFactory::Instance().Create(DYNAMICSTITCH);
  domi::Status status = kernel->Compute(op_desc_ptr, input, v_output);
  EXPECT_EQ(status, ge::SUCCESS);
  double *output_data = const_cast<double *>(reinterpret_cast<const double *>(v_output[0]->GetData().data()));
  EXPECT_DOUBLE_EQ(output_data[0], 4);
  EXPECT_DOUBLE_EQ(output_data[1], 3);
  EXPECT_DOUBLE_EQ(output_data[2], 5);
  EXPECT_DOUBLE_EQ(output_data[3], 6);
  EXPECT_DOUBLE_EQ(output_data[4], 1);
}

TEST_F(UTEST_graph_passes_folding_kernel_dynamic_stitch_kernel, unsupported_data_type) {
  OpDescPtr op_desc_ptr = make_shared<OpDesc>("dynamicstitch", "DynamicStitch");
  AttrUtils::SetInt(op_desc_ptr, "DynamicStitchN_", (int64_t)2);
  vector<bool> is_input_const_vec = {true, true, true, true};
  op_desc_ptr->SetIsInputConst(is_input_const_vec);

  vector<int64_t> dims_vec_0 = {4};
  vector<int32_t> data_vec_0 = {0, 1, 2, 3};
  GeTensorDesc tensor_desc_0(GeShape(dims_vec_0), FORMAT_NCHW, DT_INT32);
  ConstGeTensorPtr tensor_0 =
      make_shared<GeTensor>(tensor_desc_0, (uint8_t *)data_vec_0.data(), data_vec_0.size() * sizeof(int32_t));

  vector<int64_t> dims_vec_1 = {};
  vector<int32_t> data_vec_1 = {4};
  GeTensorDesc tensor_desc_1(GeShape(dims_vec_1), FORMAT_NCHW, DT_INT32);
  ConstGeTensorPtr tensor_1 =
      make_shared<GeTensor>(tensor_desc_1, (uint8_t *)data_vec_1.data(), data_vec_1.size() * sizeof(int32_t));

  vector<int64_t> dims_vec_2 = {4};
  vector<string> data_vec_2 = {"4", "3", "5", "6"};
  GeTensorDesc tensor_desc_2(GeShape(dims_vec_2), FORMAT_NCHW, DT_STRING);
  ConstGeTensorPtr tensor_2 =
      make_shared<GeTensor>(tensor_desc_2, (uint8_t *)data_vec_2.data(), data_vec_2.size() * sizeof(string));

  vector<int64_t> dims_vec_3 = {};
  vector<string> data_vec_3 = {"1"};
  GeTensorDesc tensor_desc_3(GeShape(dims_vec_3), FORMAT_NCHW, DT_STRING);
  ConstGeTensorPtr tensor_3 =
      make_shared<GeTensor>(tensor_desc_3, (uint8_t *)data_vec_3.data(), data_vec_3.size() * sizeof(string));

  op_desc_ptr->AddInputDesc(tensor_desc_0);
  op_desc_ptr->AddInputDesc(tensor_desc_1);
  op_desc_ptr->AddInputDesc(tensor_desc_2);
  op_desc_ptr->AddInputDesc(tensor_desc_3);

  vector<ConstGeTensorPtr> input = {tensor_0, tensor_1, tensor_2, tensor_3};
  vector<GeTensorPtr> v_output;

  shared_ptr<Kernel> kernel = KernelFactory::Instance().Create(DYNAMICSTITCH);
  domi::Status status = kernel->Compute(op_desc_ptr, input, v_output);
  EXPECT_EQ(status, domi::NOT_CHANGED);
}

TEST_F(UTEST_graph_passes_folding_kernel_dynamic_stitch_kernel, validate_param_fail) {
  // op desc ptr is null
  vector<ConstGeTensorPtr> empty_input;
  vector<GeTensorPtr> empty_v_output;
  OpDescPtr op_desc_ptr = nullptr;
  shared_ptr<Kernel> kernel = KernelFactory::Instance().Create(DYNAMICSTITCH);
  domi::Status status = kernel->Compute(nullptr, empty_input, empty_v_output);
  EXPECT_EQ(status, ge::PARAM_INVALID);
  // input is empty
  op_desc_ptr = make_shared<OpDesc>("dynamicstitch", "DynamicStitch");
  status = kernel->Compute(op_desc_ptr, empty_input, empty_v_output);
  EXPECT_EQ(status, domi::NOT_CHANGED);

  // attr N is not exist
  vector<int64_t> dims_vec_0 = {4};
  vector<int32_t> data_vec_0 = {0, 1, 2, 3};
  GeTensorDesc tensor_desc_0(GeShape(dims_vec_0), FORMAT_NCHW, DT_INT32);
  ConstGeTensorPtr tensor_0 =
      make_shared<GeTensor>(tensor_desc_0, (uint8_t *)data_vec_0.data(), data_vec_0.size() * sizeof(int32_t));

  vector<int64_t> dims_vec_2 = {4};
  vector<int32_t> data_vec_2 = {4, 3, 5, 6};
  GeTensorDesc tensor_desc_2(GeShape(dims_vec_2), FORMAT_NCHW, DT_INT32);
  ConstGeTensorPtr tensor_1 =
      make_shared<GeTensor>(tensor_desc_2, (uint8_t *)data_vec_2.data(), data_vec_2.size() * sizeof(int32_t));

  vector<ConstGeTensorPtr> input = {tensor_0, tensor_1};
  status = kernel->Compute(op_desc_ptr, input, empty_v_output);
  EXPECT_EQ(status, domi::NOT_CHANGED);

  AttrUtils::SetInt(op_desc_ptr, "DynamicStitchN_", (int64_t)4);
  status = kernel->Compute(op_desc_ptr, input, empty_v_output);
  EXPECT_EQ(status, domi::NOT_CHANGED);
}

TEST_F(UTEST_graph_passes_folding_kernel_dynamic_stitch_kernel, repeated_indice_int32_success) {
  OpDescPtr op_desc_ptr = make_shared<OpDesc>("dynamicstitch", "DynamicStitch");
  AttrUtils::SetInt(op_desc_ptr, "DynamicStitchN_", (int64_t)2);
  vector<bool> is_input_const_vec = {true, true, true, true};
  op_desc_ptr->SetIsInputConst(is_input_const_vec);

  vector<int64_t> dims_vec_0 = {4};
  vector<int32_t> data_vec_0 = {0, 1, 2, 3};
  GeTensorDesc tensor_desc_0(GeShape(dims_vec_0), FORMAT_NCHW, DT_INT32);
  ConstGeTensorPtr tensor_0 =
      make_shared<GeTensor>(tensor_desc_0, (uint8_t *)data_vec_0.data(), data_vec_0.size() * sizeof(int32_t));

  vector<int64_t> dims_vec_1 = {2};
  vector<int32_t> data_vec_1 = {1, 2};
  GeTensorDesc tensor_desc_1(GeShape(dims_vec_1), FORMAT_NCHW, DT_INT32);
  ConstGeTensorPtr tensor_1 =
      make_shared<GeTensor>(tensor_desc_1, (uint8_t *)data_vec_1.data(), data_vec_1.size() * sizeof(int32_t));

  vector<int64_t> dims_vec_2 = {4};
  vector<int32_t> data_vec_2 = {4, 3, 5, 6};
  GeTensorDesc tensor_desc_2(GeShape(dims_vec_2), FORMAT_NCHW, DT_INT32);
  ConstGeTensorPtr tensor_2 =
      make_shared<GeTensor>(tensor_desc_2, (uint8_t *)data_vec_2.data(), data_vec_2.size() * sizeof(int32_t));

  vector<int64_t> dims_vec_3 = {2};
  vector<int32_t> data_vec_3 = {7, 8};
  GeTensorDesc tensor_desc_3(GeShape(dims_vec_3), FORMAT_NCHW, DT_INT32);
  ConstGeTensorPtr tensor_3 =
      make_shared<GeTensor>(tensor_desc_3, (uint8_t *)data_vec_3.data(), data_vec_3.size() * sizeof(int32_t));

  op_desc_ptr->AddInputDesc(tensor_desc_0);
  op_desc_ptr->AddInputDesc(tensor_desc_1);
  op_desc_ptr->AddInputDesc(tensor_desc_2);
  op_desc_ptr->AddInputDesc(tensor_desc_3);

  vector<ConstGeTensorPtr> input = {tensor_0, tensor_1, tensor_2, tensor_3};
  vector<GeTensorPtr> v_output;

  shared_ptr<Kernel> kernel = KernelFactory::Instance().Create(DYNAMICSTITCH);
  domi::Status status = kernel->Compute(op_desc_ptr, input, v_output);
  EXPECT_EQ(status, ge::SUCCESS);
  int32_t *output_data = const_cast<int32_t *>(reinterpret_cast<const int32_t *>(v_output[0]->GetData().data()));
  EXPECT_EQ(output_data[0], 4);
  EXPECT_EQ(output_data[1], 7);
  EXPECT_EQ(output_data[2], 8);
  EXPECT_EQ(output_data[3], 6);
}

TEST_F(UTEST_graph_passes_folding_kernel_dynamic_stitch_kernel, repeated_indice_int64_success) {
  OpDescPtr op_desc_ptr = make_shared<OpDesc>("dynamicstitch", "DynamicStitch");
  AttrUtils::SetInt(op_desc_ptr, "DynamicStitchN_", (int64_t)2);
  vector<bool> is_input_const_vec = {true, true, true, true};
  op_desc_ptr->SetIsInputConst(is_input_const_vec);

  vector<int64_t> dims_vec_0 = {4};
  vector<int32_t> data_vec_0 = {0, 1, 2, 3};
  GeTensorDesc tensor_desc_0(GeShape(dims_vec_0), FORMAT_NCHW, DT_INT32);
  ConstGeTensorPtr tensor_0 =
      make_shared<GeTensor>(tensor_desc_0, (uint8_t *)data_vec_0.data(), data_vec_0.size() * sizeof(int32_t));

  vector<int64_t> dims_vec_1 = {2};
  vector<int32_t> data_vec_1 = {1, 2};
  GeTensorDesc tensor_desc_1(GeShape(dims_vec_1), FORMAT_NCHW, DT_INT32);
  ConstGeTensorPtr tensor_1 =
      make_shared<GeTensor>(tensor_desc_1, (uint8_t *)data_vec_1.data(), data_vec_1.size() * sizeof(int32_t));

  vector<int64_t> dims_vec_2 = {4};
  vector<int64_t> data_vec_2 = {4, 3, 5, 6};
  GeTensorDesc tensor_desc_2(GeShape(dims_vec_2), FORMAT_NCHW, DT_INT64);
  ConstGeTensorPtr tensor_2 =
      make_shared<GeTensor>(tensor_desc_2, (uint8_t *)data_vec_2.data(), data_vec_2.size() * sizeof(int64_t));

  vector<int64_t> dims_vec_3 = {2};
  vector<int64_t> data_vec_3 = {7, 8};
  GeTensorDesc tensor_desc_3(GeShape(dims_vec_3), FORMAT_NCHW, DT_INT64);
  ConstGeTensorPtr tensor_3 =
      make_shared<GeTensor>(tensor_desc_3, (uint8_t *)data_vec_3.data(), data_vec_3.size() * sizeof(int64_t));

  op_desc_ptr->AddInputDesc(tensor_desc_0);
  op_desc_ptr->AddInputDesc(tensor_desc_1);
  op_desc_ptr->AddInputDesc(tensor_desc_2);
  op_desc_ptr->AddInputDesc(tensor_desc_3);

  vector<ConstGeTensorPtr> input = {tensor_0, tensor_1, tensor_2, tensor_3};
  vector<GeTensorPtr> v_output;

  shared_ptr<Kernel> kernel = KernelFactory::Instance().Create(DYNAMICSTITCH);
  domi::Status status = kernel->Compute(op_desc_ptr, input, v_output);
  EXPECT_EQ(status, ge::SUCCESS);
  int64_t *output_data = const_cast<int64_t *>(reinterpret_cast<const int64_t *>(v_output[0]->GetData().data()));
  EXPECT_EQ(output_data[0], 4);
  EXPECT_EQ(output_data[1], 7);
  EXPECT_EQ(output_data[2], 8);
  EXPECT_EQ(output_data[3], 6);
}
