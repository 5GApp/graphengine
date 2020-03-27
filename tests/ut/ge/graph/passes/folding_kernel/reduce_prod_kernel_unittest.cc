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
#include "graph/passes/folding_kernel/reduce_prod_kernel.h"

#include "common/debug/log.h"
#include "common/debug/memory_dumper.h"
#include "common/op/ge_op_utils.h"
#include "common/types.h"
#include "graph/passes/folding_kernel/concat_v2_kernel.h"
#include "graph/passes/folding_kernel/kernel_utils.h"
#include "graph/types.h"
#include "graph/utils/attr_utils.h"
#include "graph/utils/graph_utils.h"
#include "graph/utils/op_desc_utils.h"
#include "graph/utils/tensor_utils.h"
#include "inc/kernel_factory.h"
#undef protected
#undef private

using namespace std;
using namespace testing;
using namespace domi;
using namespace ge;

class UTEST_graph_passes_folding_kernel_reduce_prod_kernel : public testing::Test {
 protected:
  void SetUp() {}

  void TearDown() {}
};

TEST_F(UTEST_graph_passes_folding_kernel_reduce_prod_kernel, int32_success) {
  OpDescPtr op_desc_ptr = make_shared<OpDesc>("ReduceProd", REDUCEPROD);

  vector<int64_t> dims_vec_0 = {2, 3};
  vector<int32_t> data_vec_0 = {1, 2, 5, 7, 3, 1};
  GeTensorDesc tensor_desc_0(GeShape(dims_vec_0), FORMAT_NCHW, DT_INT32);
  ConstGeTensorPtr tensor_0 =
      make_shared<GeTensor>(tensor_desc_0, (uint8_t *)data_vec_0.data(), data_vec_0.size() * sizeof(int32_t));

  vector<int64_t> dims_vec_1 = {1};
  vector<int32_t> data_vec_1 = {0};
  GeTensorDesc tensor_desc_1(GeShape(dims_vec_1), FORMAT_NCHW, DT_INT32);
  ConstGeTensorPtr tensor_1 =
      make_shared<GeTensor>(tensor_desc_1, (uint8_t *)data_vec_1.data(), data_vec_1.size() * sizeof(int32_t));

  vector<ConstGeTensorPtr> input = {tensor_0, tensor_1};
  vector<GeTensorPtr> v_output;

  shared_ptr<Kernel> kernel = KernelFactory::Instance().Create(REDUCEPROD);
  domi::Status status = kernel->Compute(op_desc_ptr, input, v_output);

  int32_t *out_data = (int32_t *)v_output[0]->GetData().data();

  EXPECT_EQ(domi::SUCCESS, status);
  EXPECT_EQ(out_data[0], 7);
  EXPECT_EQ(out_data[1], 6);
  EXPECT_EQ(out_data[2], 5);
  EXPECT_EQ(v_output[0]->GetData().size(), 12);
}

TEST_F(UTEST_graph_passes_folding_kernel_reduce_prod_kernel, double_notchanged) {
  OpDescPtr op_desc_ptr = make_shared<OpDesc>("ReduceProd", REDUCEPROD);

  vector<int64_t> dims_vec_0 = {2, 3};
  vector<int32_t> data_vec_0 = {1, 2, 5, 7, 3, 1};
  GeTensorDesc tensor_desc_0(GeShape(dims_vec_0), FORMAT_NCHW, DT_DOUBLE);
  ConstGeTensorPtr tensor_0 =
      make_shared<GeTensor>(tensor_desc_0, (uint8_t *)data_vec_0.data(), data_vec_0.size() * sizeof(int32_t));

  vector<int64_t> dims_vec_1 = {1};
  vector<int32_t> data_vec_1 = {0};
  GeTensorDesc tensor_desc_1(GeShape(dims_vec_1), FORMAT_NCHW, DT_DOUBLE);
  ConstGeTensorPtr tensor_1 =
      make_shared<GeTensor>(tensor_desc_1, (uint8_t *)data_vec_1.data(), data_vec_1.size() * sizeof(int32_t));

  vector<ConstGeTensorPtr> input = {tensor_0, tensor_1};
  vector<GeTensorPtr> v_output;

  shared_ptr<Kernel> kernel = KernelFactory::Instance().Create(REDUCEPROD);
  domi::Status status = kernel->Compute(op_desc_ptr, input, v_output);

  EXPECT_EQ(domi::NOT_CHANGED, status);
}

TEST_F(UTEST_graph_passes_folding_kernel_reduce_prod_kernel, invalid_input_size_notchanged) {
  OpDescPtr op_desc_ptr = make_shared<OpDesc>("ReduceProd", REDUCEPROD);

  vector<int64_t> dims_vec_0 = {2, 3};
  vector<int32_t> data_vec_0 = {1, 2, 5, 7, 3, 1};
  GeTensorDesc tensor_desc_0(GeShape(dims_vec_0), FORMAT_NCHW, DT_DOUBLE);
  ConstGeTensorPtr tensor_0 =
      make_shared<GeTensor>(tensor_desc_0, (uint8_t *)data_vec_0.data(), data_vec_0.size() * sizeof(int32_t));

  vector<int64_t> dims_vec_1 = {1};
  vector<int32_t> data_vec_1 = {0};
  GeTensorDesc tensor_desc_1(GeShape(dims_vec_1), FORMAT_NCHW, DT_DOUBLE);
  ConstGeTensorPtr tensor_1 =
      make_shared<GeTensor>(tensor_desc_1, (uint8_t *)data_vec_1.data(), data_vec_1.size() * sizeof(int32_t));

  vector<ConstGeTensorPtr> input = {tensor_0, tensor_1, tensor_1};
  vector<ConstGeTensorPtr> input2 = {tensor_0};
  vector<GeTensorPtr> v_output;
  shared_ptr<Kernel> kernel = KernelFactory::Instance().Create(REDUCEPROD);
  domi::Status status = kernel->Compute(op_desc_ptr, input, v_output);
  domi::Status status2 = kernel->Compute(op_desc_ptr, input2, v_output);

  EXPECT_EQ(domi::NOT_CHANGED, status);
  EXPECT_EQ(domi::NOT_CHANGED, status2);
}

TEST_F(UTEST_graph_passes_folding_kernel_reduce_prod_kernel, invalid_axis_dim_notchanged) {
  OpDescPtr op_desc_ptr = make_shared<OpDesc>("ReduceProd", REDUCEPROD);

  vector<int64_t> dims_vec_0 = {2, 3};
  vector<int32_t> data_vec_0 = {1, 2, 5, 7, 3, 1};
  GeTensorDesc tensor_desc_0(GeShape(dims_vec_0), FORMAT_NCHW, DT_DOUBLE);
  ConstGeTensorPtr tensor_0 =
      make_shared<GeTensor>(tensor_desc_0, (uint8_t *)data_vec_0.data(), data_vec_0.size() * sizeof(int32_t));

  vector<int64_t> dims_vec_1 = {1, 1, 2};
  vector<int32_t> data_vec_1 = {0, 1};
  GeTensorDesc tensor_desc_1(GeShape(dims_vec_1), FORMAT_NCHW, DT_DOUBLE);
  ConstGeTensorPtr tensor_1 =
      make_shared<GeTensor>(tensor_desc_1, (uint8_t *)data_vec_1.data(), data_vec_1.size() * sizeof(int32_t));

  vector<ConstGeTensorPtr> input = {tensor_0, tensor_1};
  vector<GeTensorPtr> v_output;
  shared_ptr<Kernel> kernel = KernelFactory::Instance().Create(REDUCEPROD);
  domi::Status status = kernel->Compute(op_desc_ptr, input, v_output);

  EXPECT_EQ(domi::NOT_CHANGED, status);
}

TEST_F(UTEST_graph_passes_folding_kernel_reduce_prod_kernel, invalid_axis_notchanged) {
  OpDescPtr op_desc_ptr = make_shared<OpDesc>("ReduceProd", REDUCEPROD);

  vector<int64_t> dims_vec_0 = {2, 3};
  vector<int32_t> data_vec_0 = {1, 2, 5, 7, 3, 1};
  GeTensorDesc tensor_desc_0(GeShape(dims_vec_0), FORMAT_NCHW, DT_DOUBLE);
  ConstGeTensorPtr tensor_0 =
      make_shared<GeTensor>(tensor_desc_0, (uint8_t *)data_vec_0.data(), data_vec_0.size() * sizeof(int32_t));

  vector<int64_t> dims_vec_1 = {1};
  vector<int32_t> data_vec_1 = {3};
  GeTensorDesc tensor_desc_1(GeShape(dims_vec_1), FORMAT_NCHW, DT_DOUBLE);
  ConstGeTensorPtr tensor_1 =
      make_shared<GeTensor>(tensor_desc_1, (uint8_t *)data_vec_1.data(), data_vec_1.size() * sizeof(int32_t));

  vector<ConstGeTensorPtr> input = {tensor_0, tensor_1};
  vector<GeTensorPtr> v_output;
  shared_ptr<Kernel> kernel = KernelFactory::Instance().Create(REDUCEPROD);
  domi::Status status = kernel->Compute(op_desc_ptr, input, v_output);

  EXPECT_EQ(domi::NOT_CHANGED, status);
}

TEST_F(UTEST_graph_passes_folding_kernel_reduce_prod_kernel, overflow_data_notchanged) {
  OpDescPtr op_desc_ptr = make_shared<OpDesc>("ReduceProd", REDUCEPROD);

  vector<int64_t> dims_vec_0 = {2, 3, 1};
  vector<int32_t> data_vec_0 = {2, 2, 2, 214748, 214748, 214748};
  GeTensorDesc tensor_desc_0(GeShape(dims_vec_0), FORMAT_NCHW, DT_DOUBLE);
  ConstGeTensorPtr tensor_0 =
      make_shared<GeTensor>(tensor_desc_0, (uint8_t *)data_vec_0.data(), data_vec_0.size() * sizeof(int32_t));

  vector<int64_t> dims_vec_1 = {1};
  vector<int32_t> data_vec_1 = {1};
  GeTensorDesc tensor_desc_1(GeShape(dims_vec_1), FORMAT_NCHW, DT_DOUBLE);
  ConstGeTensorPtr tensor_1 =
      make_shared<GeTensor>(tensor_desc_1, (uint8_t *)data_vec_1.data(), data_vec_1.size() * sizeof(int32_t));

  vector<ConstGeTensorPtr> input = {tensor_0, tensor_1};
  vector<GeTensorPtr> v_output;
  shared_ptr<Kernel> kernel = KernelFactory::Instance().Create(REDUCEPROD);
  domi::Status status = kernel->Compute(op_desc_ptr, input, v_output);

  EXPECT_EQ(domi::NOT_CHANGED, status);
}
