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

#include "graph/load/new_model_manager/task_info/kernel_ex_task_info.h"

#include <vector>

#include "cce/aicpu_engine_struct.h"
#include "cce/fwk_adpt_struct.h"
#include "common/ge/ge_util.h"
#include "common/properties_manager.h"
#include "framework/common/debug/ge_log.h"
#include "framework/common/fmk_error_codes.h"
#include "graph/attr_value.h"
#include "graph/load/new_model_manager/davinci_model.h"
#include "graph/load/new_model_manager/model_manager.h"

namespace ge {
static const char *const GE_GLOBAL_STEP = "Variable";

Status KernelExTaskInfo::Init(const domi::TaskDef &task_def, DavinciModel *davinci_model) {
  GELOGI("KernelExTaskInfo Init Start.");
  if (davinci_model == nullptr) {
    GELOGE(PARAM_INVALID, "davinci_model is null!");
    return PARAM_INVALID;
  }

  Status ret = SetStream(task_def.stream_id(), davinci_model->GetStreamList());
  if (ret != SUCCESS) {
    return ret;
  }

  auto kernel_ex_def = task_def.kernel_ex();

  // 1. Copy context from kernelExDef.private to workspace
  uint32_t op_index = kernel_ex_def.op_index();
  OpDescPtr op_desc = davinci_model->GetOpByIndex(op_index);
  if (op_desc == nullptr) {
    GELOGE(INTERNAL_ERROR, "Init aicpu task info error, index is out of range!");
    return INTERNAL_ERROR;
  }

  vector<void *> workspace_data_addrs = ModelUtils::GetWorkspaceDataAddrs(davinci_model->GetRuntimeParam(), op_desc);
  if (workspace_data_addrs.empty()) {
    GELOGE(FAILED, "workspace_data_addrs is empty.");
    return FAILED;
  } else {
    rtError_t rt_ret =
        rtMemcpy(workspace_data_addrs[0], kernel_ex_def.task_info_size(), kernel_ex_def.task_info().data(),
                 kernel_ex_def.task_info_size(), RT_MEMCPY_HOST_TO_DEVICE);
    GE_IF_BOOL_EXEC(rt_ret != RT_ERROR_NONE, GELOGE(FAILED, "rtMemcpy error: 0x%X", rt_ret); return FAILED);
  }

  // 2. Reconstruct kernelExDef.args to STR_FWK_OP_KERNEL
  STR_FWK_OP_KERNEL fwk_op_kernel;
  if (sizeof(STR_FWK_OP_KERNEL) != kernel_ex_def.args_size()) {
    GELOGE(FAILED, "sizeof STR_FWK_OP_KERNEL is: %zu, but args_size is: %u", sizeof(STR_FWK_OP_KERNEL),
           kernel_ex_def.args_size());
    return FAILED;
  }
  errno_t sec_ret =
      memcpy_s(&fwk_op_kernel, sizeof(STR_FWK_OP_KERNEL), kernel_ex_def.args().data(), kernel_ex_def.args_size());
  if (sec_ret != EOK) {
    GELOGE(FAILED, "memcpy failed, ret: %d", sec_ret);
    return FAILED;
  }

  // 2.1 get loop cond variable for tensor array write
  uint64_t loop_cond_addr = 0;
  OpDescPtr loop_cond_node = davinci_model->GetVariableOp(GE_GLOBAL_STEP);
  if (loop_cond_node != nullptr) {
    vector<void *> v_loop_cond_addr = ModelUtils::GetOutputDataAddrs(davinci_model->GetRuntimeParam(), loop_cond_node);
    if (v_loop_cond_addr.size() != 0) {
      loop_cond_addr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(v_loop_cond_addr[0]));
    }
  }

  // 3. Set workspaceaddr, inputOutputDataAddr
  uint64_t workspace_base_addr = reinterpret_cast<uint64_t>(workspace_data_addrs[0]);
  vector<void *> input_addrs = ModelUtils::GetInputDataAddrs(davinci_model->GetRuntimeParam(), op_desc);
  vector<void *> output_addrs = ModelUtils::GetOutputDataAddrs(davinci_model->GetRuntimeParam(), op_desc);
  vector<void *> io_addrs;
  io_addrs.insert(io_addrs.end(), input_addrs.begin(), input_addrs.end());
  io_addrs.insert(io_addrs.end(), output_addrs.begin(), output_addrs.end());

  auto addrs_size = sizeof(uint64_t) * (io_addrs.size());
  if (addrs_size > 0) {
    rtError_t rt_ret = rtMalloc(&input_output_addr_, addrs_size, RT_MEMORY_HBM);
    GE_IF_BOOL_EXEC(rt_ret != RT_ERROR_NONE, GELOGE(RT_FAILED, "rtMalloc error, ret: 0x%X", rt_ret); return RT_FAILED;)

    rt_ret = rtMemcpy(input_output_addr_, addrs_size, io_addrs.data(), addrs_size, RT_MEMCPY_HOST_TO_DEVICE);
    GE_IF_BOOL_EXEC(rt_ret != RT_ERROR_NONE, GELOGE(rt_ret, "rtMemcpy to input_output_addr_ error: 0x%X", rt_ret);
                    return FAILED;)

    if (PropertiesManager::Instance().IsLayerNeedDump(davinci_model->Name(), op_desc->GetName())) {
      dump_flag_ = RT_KERNEL_DUMPFLAG;
      dump_args_ = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(input_output_addr_) +
                                            sizeof(void *) * input_addrs.size());
    }
  }

  uint64_t input_output_addr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(input_output_addr_));
  fwk_op_kernel.fwkKernelBase.fwk_kernel.workspaceBaseAddr = workspace_base_addr;
  fwk_op_kernel.fwkKernelBase.fwk_kernel.inputOutputAddr = input_output_addr;
  fwk_op_kernel.fwkKernelBase.fwk_kernel.stepIDAddr = loop_cond_addr;

  // 4. Create session
  auto session_id = fwk_op_kernel.fwkKernelBase.fwk_kernel.sessionID;
  GELOGI("session_id: %lu", session_id);
  GE_CHECK_NOTNULL(ModelManager::GetInstance());
  GE_IF_BOOL_EXEC(ModelManager::GetInstance()->CreateAicpuSession(session_id) != SUCCESS,
                  GELOGE(ret, "CreateAicpuSession error.");
                  return ret;)

  // 5. Return result
  rtError_t rt_ret = rtMalloc(&kernel_buf_, sizeof(STR_FWK_OP_KERNEL), RT_MEMORY_HBM);
  GE_IF_BOOL_EXEC(rt_ret != RT_ERROR_NONE, GELOGE(rt_ret, "rtMalloc error: 0x%X", rt_ret); return FAILED;)

  rt_ret = rtMemcpy(kernel_buf_, sizeof(STR_FWK_OP_KERNEL), static_cast<void *>(&fwk_op_kernel),
                    sizeof(STR_FWK_OP_KERNEL), RT_MEMCPY_HOST_TO_DEVICE);
  GE_IF_BOOL_EXEC(rt_ret != RT_ERROR_NONE, GELOGE(rt_ret, "rtMemcpy error, ret: Ox%X", rt_ret); return FAILED;)
  davinci_model->SetZeroCopyAddr(io_addrs, input_output_addr_);

  kernel_buf_size_ = sizeof(STR_FWK_OP_KERNEL);
  davinci_model_ = davinci_model;
  return SUCCESS;
}

Status KernelExTaskInfo::Distribute() {
  GELOGI("KernelExTaskInfo Distribute Start.");
  rtError_t rt_ret = rtKernelLaunchEx(kernel_buf_, kernel_buf_size_, dump_flag_, stream_);
  if (rt_ret != RT_ERROR_NONE) {
    GELOGE(RT_FAILED, "Call rt api failed, ret: 0x%X", rt_ret);
    return RT_FAILED;
  }

  if (davinci_model_ == nullptr) {
    GELOGE(PARAM_INVALID, "davinci_model_ is null.");
    return PARAM_INVALID;
  }

  uint32_t taskid = 0;
  rt_ret = rtModelGetTaskId(davinci_model_->GetRtModelHandle(), &taskid);
  if (rt_ret != RT_ERROR_NONE) {
    GELOGE(RT_FAILED, "Call rt api failed, ret: 0x%X", rt_ret);
    return RT_FAILED;
  }
  task_id_ = taskid;

  return SUCCESS;
}

Status KernelExTaskInfo::Release() {
  Status ret = SUCCESS;
  if (kernel_buf_ != nullptr) {
    rtError_t rt_ret = rtFree(kernel_buf_);
    if (rt_ret != RT_ERROR_NONE) {
      GELOGW("rtFree error, ret: 0x%X", rt_ret);
      ret = FAILED;
    } else {
      kernel_buf_ = nullptr;
    }
  }
  if (input_output_addr_ != nullptr) {
    rtError_t rt_ret = rtFree(input_output_addr_);
    if (rt_ret != RT_ERROR_NONE) {
      GELOGW("rtFree error, ret: 0x%X", rt_ret);
      ret = FAILED;
    } else {
      input_output_addr_ = nullptr;
    }
  }
  return ret;
}

REGISTER_TASK_INFO(RT_MODEL_TASK_KERNEL_EX, KernelExTaskInfo);
}  // namespace ge
