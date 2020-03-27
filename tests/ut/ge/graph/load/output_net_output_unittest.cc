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
#include <memory>

#include "securec.h"

#define protected public
#define private public
#include "common/debug/memory_dumper.h"
#include "common/op/ge_op_utils.h"
#include "graph/load/new_model_manager/davinci_model.h"
#include "graph/load/new_model_manager/model_output.h"
#include "graph/load/new_model_manager/model_utils.h"
#include "graph/load/output/output.h"
#include "graph/manager/graph_var_manager.h"
#include "new_op_test_utils.h"
#include "proto/om.pb.h"
using namespace std;
using namespace domi;

namespace ge {
class UTEST_net_output : public testing::Test {
 protected:
  void TearDown() {}
  shared_ptr<OmeTestOpDescBuilder> GenOpdef(OpDescPtr &op_desc, int flag) {
    shared_ptr<OmeTestOpDescBuilder> builder = make_shared<OmeTestOpDescBuilder>(op_desc);
    builder->SetStreamId(0);
    builder->AddInput(1);
    builder->SetType("NetOutput");

    if (flag == 1) {
      auto input_desc_1 = builder->AddInputDesc({1, 1, 10, 10}, FORMAT_NCHW, DT_FLOAT16);
    }
    auto input_desc_1 = builder->AddInputDesc({1, 1, 10, 10}, FORMAT_NCHW, DT_FLOAT16);

    if (flag == 2) {
      auto input_desc_2 = builder->AddInputDesc({1, 1, 10, 10}, FORMAT_NCHW, DT_FLOAT16);
    }
    if (flag == 3) {
      builder->AddInput(10);
    }

    return builder;
  }
  shared_ptr<OmeTestOpDescBuilder> GenOpdef2(OpDescPtr &op_desc) {
    shared_ptr<OmeTestOpDescBuilder> builder = make_shared<OmeTestOpDescBuilder>(op_desc);
    builder->SetStreamId(0);
    builder->SetType("NetOutput");
    builder->AddInput(10);

    auto input_desc_1 = builder->AddInputDesc({64, 32, 5, 5}, FORMAT_FRACTAL_Z, DT_FLOAT);

    builder->AddInput(1000000);
    auto input_desc_2 = builder->AddInputDesc({1, 10, 10, 1}, FORMAT_NHWC, DT_FLOAT);

    builder->AddOutput(2000000);
    auto output_desc_1 = builder->AddOutputDesc({64, 32, 5, 5}, FORMAT_NCHW, DT_FLOAT);

    builder->AddOutput(2100000);
    output_desc_1 = builder->AddOutputDesc({1, 10, 10, 1}, FORMAT_NHWC, DT_FLOAT);

    return builder;
  }

 public:
  shared_ptr<DavinciModel> dav_model_;
};

TEST_F(UTEST_net_output, TestGetInputSize) {
  shared_ptr<OpDesc> custom_op_desc = make_shared<OpDesc>();
  OmeTestOpDescBuilder builder(custom_op_desc);
  builder.SetName("netoutput");
  builder.SetStreamId(0);
  builder.SetType("NetOutput");

  auto input_desc_1 = builder.AddInputDesc({1, 1, 1, 1}, FORMAT_FRACTAL_Z, DT_FLOAT);
  builder.AddInput(1);
  auto output_desc = builder.AddOutputDesc({1, 1, 1, 1}, FORMAT_NCHW, DT_FLOAT);
  builder.AddOutput(1);
  builder.Finish();

  vector<uint32_t> v_output_size = ModelUtils::GetInputSize(custom_op_desc);
  EXPECT_EQ(v_output_size.size(), 1);
}

// test ModelUtils::IsOutput
TEST_F(UTEST_net_output, success_IsOutput) {
  ModelUtils *modelUtils = new ModelUtils();
  std::shared_ptr<OpDesc> op_desc = std::make_shared<OpDesc>();
  OmeTestOpDescBuilder builder(op_desc);
  builder.SetType("NetOutput");
  vector<GeTensorDescPtr> outputsDesc;
  std::shared_ptr<GeTensorDesc> desc = std::make_shared<GeTensorDesc>();
  outputsDesc.push_back(desc);
  op_desc->outputs_desc_ = outputsDesc;
  bool ret = modelUtils->IsOutput(op_desc);
  EXPECT_EQ(false, ret);

  delete modelUtils;
}

// test ModelUtils::IsOutput
TEST_F(UTEST_net_output, true_IsOutput) {
  ModelUtils *modelUtils = new ModelUtils();
  std::shared_ptr<OpDesc> op_desc = std::make_shared<OpDesc>();
  OmeTestOpDescBuilder builder(op_desc);
  builder.SetType("NetOutput");
  vector<GeTensorDescPtr> outputsDesc;
  std::shared_ptr<GeTensorDesc> desc = std::make_shared<GeTensorDesc>();
  outputsDesc.push_back(desc);
  op_desc->outputs_desc_ = outputsDesc;
  ge::TensorUtils::SetOutputTensor(*(outputsDesc[0].get()), true);
  bool ret = modelUtils->IsOutput(op_desc);
  EXPECT_EQ(true, ret);

  delete modelUtils;
}

// test ModelUtils::IsInputTensorNeedTrans
TEST_F(UTEST_net_output, success_IsInputTensorNeedTrans) {
  ModelUtils *modelUtils = new ModelUtils();
  std::shared_ptr<OpDesc> op_desc = std::make_shared<OpDesc>();
  OmeTestOpDescBuilder builder(op_desc);
  builder.SetType("NetOutput");
  size_t tensor_index = 1;
  vector<GeTensorDescPtr> outputsDesc;
  std::shared_ptr<GeTensorDesc> desc = std::make_shared<GeTensorDesc>();
  outputsDesc.push_back(desc);
  op_desc->outputs_desc_ = outputsDesc;
  op_desc->inputs_desc_ = outputsDesc;

  bool ret = modelUtils->IsInputTensorNeedTrans(op_desc, tensor_index);
  EXPECT_EQ(false, ret);

  delete modelUtils;
}

// test ModelUtils::GetOutputSize
TEST_F(UTEST_net_output, success_GetOutputSize) {
  vector<uint32_t> v_output_size;

  ModelUtils *modelUtils = new ModelUtils();
  std::shared_ptr<OpDesc> op_desc = std::make_shared<OpDesc>();
  vector<GeTensorDescPtr> outputsDesc;
  std::shared_ptr<GeTensorDesc> desc = std::make_shared<GeTensorDesc>();
  outputsDesc.push_back(desc);
  op_desc->outputs_desc_ = outputsDesc;
  EXPECT_EQ(v_output_size, modelUtils->GetOutputSize(op_desc));

  vector<int64_t> output = {1};
  op_desc->SetOutputOffset(output);
  uint32_t tensor_size = 0;
  v_output_size.push_back(tensor_size);
  EXPECT_EQ(v_output_size, modelUtils->GetOutputSize(op_desc));
  delete modelUtils;
}

// test ModelUtils::GetWorkspaceSize
TEST_F(UTEST_net_output, success_GetWorkspaceSize) {
  vector<uint32_t> v_workspace_size;

  ModelUtils *modelUtils = new ModelUtils();
  std::shared_ptr<OpDesc> op_desc = std::make_shared<OpDesc>();
  vector<int64_t> workspace = {1};
  op_desc->SetWorkspace(workspace);
  EXPECT_EQ(v_workspace_size, modelUtils->GetWorkspaceSize(op_desc));

  op_desc->SetWorkspaceBytes(workspace);
  v_workspace_size.push_back(1);
  EXPECT_EQ(v_workspace_size, modelUtils->GetWorkspaceSize(op_desc));
  delete modelUtils;
}

// test ModelUtils::GetWeightSize
TEST_F(UTEST_net_output, success_GetWeightSize) {
  vector<uint32_t> v_weight_size;

  ModelUtils *modelUtils = new ModelUtils();
  std::shared_ptr<OpDesc> op_desc = std::make_shared<OpDesc>();
  op_desc->SetType("Const");
  EXPECT_EQ(v_weight_size, modelUtils->GetWeightSize(op_desc));

  op_desc->SetType("NetOutput");
  vector<GeTensorDescPtr> inputsDesc;
  std::shared_ptr<GeTensorDesc> desc = std::make_shared<GeTensorDesc>();
  inputsDesc.push_back(desc);
  op_desc->inputs_desc_ = inputsDesc;

  vector<bool> is_input_const = {true};
  op_desc->SetIsInputConst(is_input_const);
  v_weight_size.push_back(0);
  EXPECT_EQ(v_weight_size, modelUtils->GetWeightSize(op_desc));

  delete modelUtils;
}

// test ModelUtils::GetWeights
TEST_F(UTEST_net_output, success_GetWeights) {
  vector<ConstGeTensorPtr> v_weights;

  ModelUtils *modelUtils = new ModelUtils();
  std::shared_ptr<OpDesc> op_desc = std::make_shared<OpDesc>();
  op_desc->SetType("Const");
  EXPECT_EQ(v_weights, modelUtils->GetWeights(op_desc));

  op_desc->SetType("NetOutput");
  vector<GeTensorDescPtr> inputsDesc;
  std::shared_ptr<GeTensorDesc> desc = std::make_shared<GeTensorDesc>();
  inputsDesc.push_back(desc);
  op_desc->inputs_desc_ = inputsDesc;

  vector<bool> is_input_const = {true};
  op_desc->SetIsInputConst(is_input_const);
  GeTensorDesc tensor_desc;
  EXPECT_EQ(v_weights, modelUtils->GetWeights(op_desc));

  delete modelUtils;
}

// test ModelUtils::GetInputDescs
TEST_F(UTEST_net_output, success_GetInputDescs) {
  vector<::opTensor_t> v_input_descs;
  vector<::tagCcAICPUTensor> ret;
  ModelUtils *modelUtils = new ModelUtils();
  std::shared_ptr<OpDesc> op_desc = std::make_shared<OpDesc>();
  ret = modelUtils->GetInputDescs(op_desc);
  EXPECT_EQ(v_input_descs.size(), ret.size());

  vector<GeTensorDescPtr> inputsDesc;
  std::shared_ptr<GeTensorDesc> desc = std::make_shared<GeTensorDesc>();
  inputsDesc.push_back(desc);
  op_desc->inputs_desc_ = inputsDesc;
  vector<bool> is_input_const = {false};
  op_desc->SetIsInputConst(is_input_const);

  opTensor_t tmp;
  tmp.format = OP_TENSOR_FORMAT_NC1HWC0;
  tmp.dim_cnt = 0;
  tmp.data_type = OP_DATA_FLOAT;
  v_input_descs.push_back(tmp);
  ret = modelUtils->GetInputDescs(op_desc);
  EXPECT_EQ(v_input_descs.size(), ret.size());

  delete modelUtils;
}

// test ModelUtils::GetOutputDescs
TEST_F(UTEST_net_output, success_GetOutputDescs) {
  vector<::opTensor_t> v_output_descs;
  vector<::tagCcAICPUTensor> ret;
  ModelUtils *modelUtils = new ModelUtils();
  std::shared_ptr<OpDesc> op_desc = std::make_shared<OpDesc>();
  ret = modelUtils->GetOutputDescs(op_desc);
  EXPECT_EQ(v_output_descs.size(), ret.size());

  vector<GeTensorDescPtr> outputsDesc;
  std::shared_ptr<GeTensorDesc> desc = std::make_shared<GeTensorDesc>();
  outputsDesc.push_back(desc);
  op_desc->outputs_desc_ = outputsDesc;

  opTensor_t tmp;
  tmp.format = OP_TENSOR_FORMAT_NC1HWC0;
  tmp.dim_cnt = 0;
  tmp.data_type = OP_DATA_FLOAT;
  v_output_descs.push_back(tmp);
  ret = modelUtils->GetOutputDescs(op_desc);
  EXPECT_EQ(v_output_descs.size(), ret.size());

  delete modelUtils;
}

// test Output::GetOutputData
TEST_F(UTEST_net_output, success_GetOutputData) {
  Output *output = new Output(nullptr, nullptr);
  output->v_input_data_addr_.push_back((void *)1);
  output->v_input_size_.push_back(1);
  output->input_num_ = 1;

  vector<void *> v_data_addr;
  vector<uint32_t> v_data_size;
  output->GetOutputData(v_data_addr, v_data_size);

  EXPECT_EQ(output->v_input_data_addr_, v_data_addr);
  EXPECT_EQ(output->v_input_size_, v_data_size);
  delete output;
}
}  // namespace ge
