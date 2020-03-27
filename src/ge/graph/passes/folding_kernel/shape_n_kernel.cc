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

#include "graph/passes/folding_kernel/shape_n_kernel.h"

#include <memory>

#include "framework/common/debug/ge_log.h"
#include "framework/common/ge_inner_error_codes.h"
#include "graph/passes/folding_kernel/kernel_utils.h"
#include "graph/passes/pass_utils.h"
#include "inc/kernel_factory.h"

using domi::NOT_CHANGED;

namespace ge {
Status ShapeNKernel::Compute(const NodePtr &node, std::vector<GeTensorPtr> &v_output) {
  GELOGD("ShapeN kernel in");
  if (node == nullptr) {
    GELOGE(FAILED, "parameter is null.");
    return FAILED;
  }
  OpDescPtr op_desc = node->GetOpDesc();
  GE_CHECK_NOTNULL(op_desc);
  if (op_desc->GetAllInputsDesc().size() != op_desc->GetAllOutputsDesc().size()) {
    GELOGW("ShapeN kernel, input and output are not the same size. Input size:%zu, output size:%zu",
           op_desc->GetAllInputsDesc().size(), op_desc->GetAllOutputsDesc().size());
    return NOT_CHANGED;
  }

  for (size_t i = 0; i < op_desc->GetAllInputsDesc().size(); i++) {
    if (KernelUtils::IsUnknownShape(op_desc->GetInputDesc(i).GetShape())) {
      GELOGW("Input %zu shape is unknown, ignore shape_n kernel.", i);
      return NOT_CHANGED;
    }
    vector<int64_t> dims = op_desc->GetInputDesc(i).GetShape().GetDims();
    Status ret = PassUtils::ConstructTensorDescWithData(op_desc->GetOutputDesc(i), dims, v_output);
    if (ret != SUCCESS) {
      GELOGE(PARAM_INVALID, "ShapeN kernel construct tensor desc failed, i:%zu", i);
      return ret;
    }
  }

  GELOGD("ShapeN kernel success");
  return SUCCESS;
}

REGISTER_KERNEL(SHAPEN, ShapeNKernel);
}  // namespace ge
