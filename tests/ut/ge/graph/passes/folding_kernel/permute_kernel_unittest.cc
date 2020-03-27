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
#include "graph/passes/folding_kernel/permute_kernel.h"

#include "common/debug/log.h"
#include "common/debug/memory_dumper.h"
#include "common/fp16_t.h"
#include "common/ge_inner_error_codes.h"
#include "common/op/attr_value_util.h"
#include "common/types.h"
#include "graph/debug/ge_attr_define.h"
#include "graph/passes/dimension_compute_pass.h"
#include "graph/passes/folding_kernel/kernel_utils.h"
#include "graph/types.h"
#include "graph/utils/attr_utils.h"
#include "graph/utils/graph_utils.h"
#include "graph/utils/op_desc_utils.h"
#include "graph/utils/tensor_utils.h"
#include "graph/utils/type_utils.h"
#include "inc/kernel_factory.h"
#undef protected
#undef private

using namespace domi;
using namespace testing;
using namespace ge;

class UTEST_graph_passes_folding_kernel_permute_kernel : public testing::Test {
 protected:
  void SetUp() {}

  void TearDown() {}
};

TEST_F(UTEST_graph_passes_folding_kernel_permute_kernel, compute_nchw_to_nhwc) {
  const std::string ATTR_ORDER = "order";
  const std::string ATTR_PERM = "perm";
  OpDescPtr op_desc_ptr = make_shared<OpDesc>("Transpose", "Transpose");

  GeTensorDesc dims_tensor_desc(GeShape({1, 1, 1, 1}), FORMAT_NHWC, DT_FLOAT);
  GeTensorDesc dims_tensor_desc_in(GeShape({1, 1, 1, 1}), FORMAT_NCHW, DT_FLOAT);
  op_desc_ptr->AddInputDesc(dims_tensor_desc_in);
  op_desc_ptr->AddOutputDesc(dims_tensor_desc);
  vector<int64_t> perm_list{0, 3, 1, 2};
  AttrUtils::SetListInt(op_desc_ptr, ATTR_ORDER, perm_list);
  vector<int64_t> dims_vec_0 = {1, 1, 1, 1};
  vector<int32_t> data_vec_0 = {1};
  GeTensorDesc tensor_desc_0(GeShape(dims_vec_0), FORMAT_NCHW, DT_FLOAT);
  ConstGeTensorPtr tensor_0 =
      make_shared<GeTensor>(tensor_desc_0, (uint8_t *)data_vec_0.data(), data_vec_0.size() * sizeof(float));

  vector<ConstGeTensorPtr> input = {tensor_0};
  vector<GeTensorPtr> v_output;

  shared_ptr<Kernel> kernel = KernelFactory::Instance().Create(TRANSPOSE);
  ge::Status status = kernel->Compute(op_desc_ptr, input, v_output);

  EXPECT_EQ(ge::SUCCESS, status);
}

TEST_F(UTEST_graph_passes_folding_kernel_permute_kernel, compute_transaxises) {
  const std::string ATTR_ORDER = "order";
  const std::string ATTR_PERM = "perm";
  OpDescPtr op_desc_ptr = make_shared<OpDesc>("Transpose", "Transpose");

  GeTensorDesc dims_tensor_desc(GeShape({1, 1, 1, 1}), FORMAT_ND, DT_FLOAT);
  GeTensorDesc dims_tensor_desc_in(GeShape({1, 1, 1, 1}), FORMAT_ND, DT_FLOAT);
  op_desc_ptr->AddInputDesc(dims_tensor_desc_in);
  op_desc_ptr->AddOutputDesc(dims_tensor_desc);
  vector<int64_t> perm_list{0, 3, 1, 2};
  AttrUtils::SetListInt(op_desc_ptr, ATTR_PERM, perm_list);
  vector<int64_t> dims_vec_0 = {1, 1, 1, 1};
  vector<int32_t> data_vec_0 = {1};
  GeTensorDesc tensor_desc_0(GeShape(dims_vec_0), FORMAT_NCHW, DT_FLOAT);
  ConstGeTensorPtr tensor_0 =
      make_shared<GeTensor>(tensor_desc_0, (uint8_t *)data_vec_0.data(), data_vec_0.size() * sizeof(float));

  vector<ConstGeTensorPtr> input = {tensor_0};
  vector<GeTensorPtr> v_output;

  shared_ptr<Kernel> kernel = KernelFactory::Instance().Create(TRANSPOSE);
  ge::Status status = kernel->Compute(op_desc_ptr, input, v_output);

  EXPECT_EQ(ge::SUCCESS, status);
}

TEST_F(UTEST_graph_passes_folding_kernel_permute_kernel, get_permlist_failed) {
  OpDescPtr op_desc_ptr = make_shared<OpDesc>("Transpose", "Transpose");

  GeTensorDesc dims_tensor_desc(GeShape({1, 1, 1, 1}), FORMAT_ND, DT_FLOAT);
  GeTensorDesc dims_tensor_desc_in(GeShape({1, 1, 1, 1}), FORMAT_NCHW, DT_FLOAT);
  op_desc_ptr->AddInputDesc(dims_tensor_desc_in);
  op_desc_ptr->AddOutputDesc(dims_tensor_desc);
  vector<int64_t> dims_vec_0 = {1, 1, 1, 1};
  vector<int32_t> data_vec_0 = {1};
  GeTensorDesc tensor_desc_0(GeShape(dims_vec_0), FORMAT_NCHW, DT_FLOAT);
  ConstGeTensorPtr tensor_0 =
      make_shared<GeTensor>(tensor_desc_0, (uint8_t *)data_vec_0.data(), data_vec_0.size() * sizeof(float));

  vector<ConstGeTensorPtr> input = {tensor_0};
  vector<GeTensorPtr> v_output;

  shared_ptr<Kernel> kernel = KernelFactory::Instance().Create(TRANSPOSE);
  ge::Status status = kernel->Compute(op_desc_ptr, input, v_output);

  EXPECT_EQ(NOT_CHANGED, status);
}

TEST_F(UTEST_graph_passes_folding_kernel_permute_kernel, compute_notsupport_inconsistent_datatype) {
  const std::string ATTR_PERM = "perm";
  OpDescPtr op_desc_ptr = make_shared<OpDesc>("Transpose", "Transpose");

  GeTensorDesc dims_tensor_desc(GeShape({1, 1, 1, 1}), FORMAT_ND, DT_FLOAT16);
  GeTensorDesc dims_tensor_desc_in(GeShape({1, 1, 1, 1}), FORMAT_ND, DT_FLOAT);
  op_desc_ptr->AddInputDesc(dims_tensor_desc_in);
  op_desc_ptr->AddOutputDesc(dims_tensor_desc);
  vector<int64_t> perm_list{0, 3, 1, 2};
  AttrUtils::SetListInt(op_desc_ptr, ATTR_PERM, perm_list);
  vector<int64_t> dims_vec_0 = {1, 1, 1, 1};
  vector<int32_t> data_vec_0 = {1};
  GeTensorDesc tensor_desc_0(GeShape(dims_vec_0), FORMAT_NCHW, DT_FLOAT);
  ConstGeTensorPtr tensor_0 =
      make_shared<GeTensor>(tensor_desc_0, (uint8_t *)data_vec_0.data(), data_vec_0.size() * sizeof(float));

  vector<ConstGeTensorPtr> input = {tensor_0};
  vector<GeTensorPtr> v_output;

  shared_ptr<Kernel> kernel = KernelFactory::Instance().Create(TRANSPOSE);
  ge::Status status = kernel->Compute(op_desc_ptr, input, v_output);

  EXPECT_EQ(NOT_CHANGED, status);
}

TEST_F(UTEST_graph_passes_folding_kernel_permute_kernel, compute_notsupport_emptyshape) {
  const std::string ATTR_ORDER = "order";
  OpDescPtr op_desc_ptr = make_shared<OpDesc>("Transpose", "Transpose");

  GeTensorDesc dims_tensor_desc(GeShape(), FORMAT_ND, DT_FLOAT);
  GeTensorDesc dims_tensor_desc_in(GeShape({1, 1, 1, 1}), FORMAT_ND, DT_FLOAT);
  op_desc_ptr->AddInputDesc(dims_tensor_desc_in);
  op_desc_ptr->AddOutputDesc(dims_tensor_desc);
  vector<int64_t> perm_list{0, 3, 1, 2};
  AttrUtils::SetListInt(op_desc_ptr, ATTR_ORDER, perm_list);
  vector<int64_t> dims_vec_0 = {1, 1, 1, 1};
  vector<int32_t> data_vec_0 = {1};
  GeTensorDesc tensor_desc_0(GeShape(dims_vec_0), FORMAT_NCHW, DT_FLOAT);
  ConstGeTensorPtr tensor_0 =
      make_shared<GeTensor>(tensor_desc_0, (uint8_t *)data_vec_0.data(), data_vec_0.size() * sizeof(float));

  vector<ConstGeTensorPtr> input = {tensor_0};
  vector<GeTensorPtr> v_output;

  shared_ptr<Kernel> kernel = KernelFactory::Instance().Create(TRANSPOSE);
  ge::Status status = kernel->Compute(op_desc_ptr, input, v_output);

  EXPECT_EQ(NOT_CHANGED, status);
}

TEST_F(UTEST_graph_passes_folding_kernel_permute_kernel, compute_param_invalid_1) {
  OpDescPtr op_desc_ptr = make_shared<OpDesc>("Transpose", "Transpose");
  auto tmp = op_desc_ptr->GetOutputDesc(0);
  tmp.SetFormat(FORMAT_NHWC);
  tmp.SetDataType(DT_FLOAT16);
  tmp.SetShape(GeShape({1, 1, 1, 1}));

  op_desc_ptr->UpdateOutputDesc(0, tmp);

  vector<int64_t> dims_vec_0 = {1, 1, 1, 1};
  vector<int32_t> data_vec_0 = {1};
  GeTensorDesc tensor_desc_0(GeShape(dims_vec_0), FORMAT_NCHW, DT_FLOAT16);
  ConstGeTensorPtr tensor_0 = make_shared<GeTensor>(tensor_desc_0, (uint8_t *)data_vec_0.data(), data_vec_0.size() * 2);

  vector<ConstGeTensorPtr> input = {};
  vector<GeTensorPtr> v_output;

  shared_ptr<Kernel> kernel = KernelFactory::Instance().Create(TRANSPOSE);
  ge::Status status = kernel->Compute(op_desc_ptr, input, v_output);
  EXPECT_EQ(ge::PARAM_INVALID, status);
}

TEST_F(UTEST_graph_passes_folding_kernel_permute_kernel, compute_param_invalid_2) {
  OpDescPtr op_desc_ptr = nullptr;

  vector<int64_t> dims_vec_0;
  vector<int32_t> data_vec_0 = {1, 1, 1, 1};
  GeTensorDesc tensor_desc_0(GeShape(dims_vec_0), FORMAT_NCHW, DT_FLOAT16);
  ConstGeTensorPtr tensor_0 = make_shared<GeTensor>(tensor_desc_0, (uint8_t *)data_vec_0.data(), data_vec_0.size() * 2);

  vector<ConstGeTensorPtr> input = {tensor_0};
  vector<GeTensorPtr> v_output;

  shared_ptr<Kernel> kernel = KernelFactory::Instance().Create(TRANSPOSE);
  ge::Status status = kernel->Compute(op_desc_ptr, input, v_output);
  EXPECT_EQ(ge::PARAM_INVALID, status);
}

TEST_F(UTEST_graph_passes_folding_kernel_permute_kernel, compute_param_invalid_3) {
  OpDescPtr op_desc_ptr = make_shared<OpDesc>("Transpose", "Transpose");
  GeTensorDesc dims_tensor_desc(GeShape({1, 1, 1, 1}), FORMAT_NHWC, DT_FLOAT);
  GeTensorDesc dims_tensor_desc_in(GeShape({1, 1, 1, 1}), FORMAT_NCHW, DT_FLOAT);
  op_desc_ptr->AddInputDesc(dims_tensor_desc_in);
  op_desc_ptr->AddOutputDesc(dims_tensor_desc);

  vector<int64_t> dims_vec_0 = {1, 1, 1, 1};
  vector<int32_t> data_vec_0 = {1, 1, 1, 1};
  GeTensorDesc tensor_desc_0(GeShape(dims_vec_0), FORMAT_NCHW, DT_FLOAT16);
  ConstGeTensorPtr tensor_0 = make_shared<GeTensor>(tensor_desc_0, (uint8_t *)data_vec_0.data(), data_vec_0.size() * 2);

  vector<ConstGeTensorPtr> input = {tensor_0};
  vector<ConstGeTensorPtr> input2 = {};
  vector<GeTensorPtr> v_output;

  shared_ptr<Kernel> kernel = KernelFactory::Instance().Create(TRANSPOSE);
  ge::Status status = kernel->Compute(op_desc_ptr, input2, v_output);
  EXPECT_EQ(ge::PARAM_INVALID, status);
}
