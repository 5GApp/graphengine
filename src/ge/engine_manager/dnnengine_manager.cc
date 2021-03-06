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

#include "engine_manager/dnnengine_manager.h"

#include <unistd.h>
#include <cstdio>
#include <fstream>
#include <map>
#include <utility>

#include "common/debug/log.h"
#include "common/ge/ge_util.h"
#include "framework/common/debug/ge_log.h"
#include "graph/ge_context.h"
#include "init/gelib.h"

namespace {
const char *const kSchedulerUnits = "schedule_units";
const char *const kId = "id";
const char *const kName = "name";
const char *const kExAttrs = "ex_attrs";
const char *const kIndependent = "independent";
const char *const kSkipAssignStream = "skip_assign_stream";
const char *const kCalEngines = "cal_engines";
const char *const kAttch = "attach";
const char *const kVectorEngine = "VectorEngine";
const char *const kAIcoreEngine = "AIcoreEngine";
const char *const kCustomOpFlag = "_custom_op_flag";
}  // namespace

namespace ge {
DNNEngineManager::DNNEngineManager() : init_flag_(false) {}
DNNEngineManager::~DNNEngineManager() {
  engines_attrs_map_.clear();
  schedulers_.clear();
}

Status DNNEngineManager::Initialize(const std::map<std::string, std::string> &options) {
  // Multiple initializations are not supported
  if (init_flag_) {
    GELOGW("DNNEngineManager has been initialized.");
    return SUCCESS;
  }

  // Load engine so
  std::string so_path = "plugin/nnengine/";
  std::string path = PluginManager::GetPath();
  path.append(so_path);
  std::string so_api_func = "GetDNNEngineObjs";
  std::vector<std::string> so_func{so_api_func};
  Status status = plugin_mgr_.Load(path, so_func);
  if (status != SUCCESS) {
    GELOGE(status, "Load engine's so failed. LibPath is %s", path.c_str());
    return status;
  }

  status = plugin_mgr_.InvokeAll<std::map<std::string, DNNEnginePtr> &>(so_api_func, engines_map_);
  if (status != SUCCESS) {
    GELOGE(status, "Get DNNEngineObjs failed.");
    return status;
  }

  GELOGI("The number of DNNEngineObjs are %zu.", engines_map_.size());

  // Engines initialize
  for (auto iter = engines_map_.begin(); iter != engines_map_.end(); ++iter) {
    if (iter->second == nullptr) {
      GELOGI("Engine: %s point to nullptr", (iter->first).c_str());
      continue;
    }

    GELOGI("DNNEngine name: %s.", (iter->first).c_str());

    status = iter->second->Initialize(options);
    if (status != SUCCESS) {
      GELOGE(status, "Engine: %s initialize failed.", (iter->first).c_str());
      return status;
    }

    // Check engines' attribute
    DNNEngineAttribute attrs;
    iter->second->GetAttributes(attrs);
    if (attrs.runtime_type == RuntimeType::DEVICE) {
      if ((attrs.mem_type.size()) != 1 || (attrs.mem_type[0] != GE_ENGINE_ATTR_MEM_TYPE_HBM)) {
        GELOGE(GE_ENG_MEMTYPE_ERROR, "Engine: %s in aicore, but the memory type is not HBM", (iter->first).c_str());
        return GE_ENG_MEMTYPE_ERROR;
      }
    }
  }

  status = ParserJsonFile();
  if (status != SUCCESS) {
    GELOGE(status, "parse json file failed");
    return status;
  }

  status = CheckJsonFile();
  if (status != SUCCESS) {
    GELOGE(status, "check json file failed");
    return status;
  }

  init_flag_ = true;

  return SUCCESS;
}

Status DNNEngineManager::Finalize() {
  // Finalize is not allowed, initialize first is necessary
  if (!init_flag_) {
    GELOGW("DNNEngineManager has been finalized.");
    return SUCCESS;
  }

  for (auto iter = engines_map_.begin(); iter != engines_map_.end(); ++iter) {
    if (iter->second != nullptr) {
      GELOGI("DNNEngine name: %s.", (iter->first).c_str());
      Status status = iter->second->Finalize();
      if (status != SUCCESS) {
        GELOGE(status, "Engine finalize failed.");
        return status;
      }
    }
  }
  init_flag_ = false;
  engines_map_.clear();
  return SUCCESS;
}

std::shared_ptr<ge::DNNEngine> DNNEngineManager::GetEngine(const std::string &name) const {
  auto iter = engines_map_.find(name);
  if (iter != engines_map_.end()) {
    return iter->second;
  }

  GELOGW("Failed to get engine object by engine name. %s.", name.c_str());
  return nullptr;
}

bool DNNEngineManager::IsEngineRegistered(const std::string &name) const {
  auto iter = engines_map_.find(name);
  if (iter != engines_map_.end()) {
    return true;
  }
  GELOGW("Engine: %s is not Registered", name.c_str());
  return false;
}

std::string DNNEngineManager::GetDNNEngineName(const OpDescPtr &op_desc) const {
  if (op_desc == nullptr) {
    GELOGE(GE_CLI_GE_NOT_INITIALIZED, "DNNEngineManager: op_desc is nullptr");
    return "";
  }
  // Use the OpsKernelManager in GELib to get the opInfos for this opCode
  std::shared_ptr<GELib> instance_ptr = ge::GELib::GetInstance();
  if ((instance_ptr == nullptr) || (!instance_ptr->InitFlag())) {
    GELOGE(GE_CLI_GE_NOT_INITIALIZED, "GetDNNEngineName failed.");
    return "";
  }
  OpsKernelManager &ops_kernel_manager = instance_ptr->OpsKernelManagerObj();
  std::vector<OpInfo> op_infos = ops_kernel_manager.GetOpsKernelInfo(op_desc->GetType());
  if (op_infos.empty()) {
    GELOGI("DNNEngineManager: Can not get op info by op type %s", op_desc->GetType().c_str());
    return "";
  }
  string ge_core_type;
  Status ret = ge::GetContext().GetOption(ge::CORE_TYPE, ge_core_type);
  if (ret != SUCCESS) {
    GELOGD("get the option CORE_TYPE fail, set it to default value VECTOR_ENGINE");
  }
  string exclude_core_Type = (ge_core_type == kVectorEngine) ? kAIcoreEngine : kVectorEngine;
  GELOGD("engine type will exclude: %s", exclude_core_Type.c_str());
  std::map<std::string, std::string> unsupported_reasons;
  for (const auto &it : op_infos) {
    if (it.engine == exclude_core_Type) {
      continue;
    }
    auto &kernel_map = ops_kernel_manager.GetAllOpsKernelInfoStores();
    auto &kernel_name = it.opKernelLib;
    auto kernel_info_store = kernel_map.find(kernel_name);
    if (kernel_info_store != kernel_map.end()) {
      std::string unsupported_reason;
      // It will be replaced by engine' checksupport
      if (kernel_info_store->second->CheckSupported(op_desc, unsupported_reason)) {
        op_desc->SetOpEngineName(it.engine);
        op_desc->SetOpKernelLibName(kernel_name);
        GELOGD("DNNEngineManager:Set OpKernelLibName %s and engine name %s into op_desc %s", kernel_name.c_str(),
               it.engine.c_str(), op_desc->GetName().c_str());
        return it.engine;
      } else {
        bool is_custom_op = false;
        if ((ge::AttrUtils::GetBool(op_desc, kCustomOpFlag, is_custom_op)) && is_custom_op) {
          GELOGE(FAILED,
                 "The custom operator registered by the user does not support the logic function delivered by this "
                 "network. Check support failed, kernel_name is %s, op type is %s, op name is %s",
                 kernel_name.c_str(), op_desc->GetType().c_str(), op_desc->GetName().c_str());
          return "";
        }
        unsupported_reasons.emplace(kernel_name, unsupported_reason);
        GELOGI("DNNEngineManager:Check support failed, kernel_name is %s, op type is %s, op name is %s",
               kernel_name.c_str(), op_desc->GetType().c_str(), op_desc->GetName().c_str());
      }
    } else {
      GELOGW(
        "DNNEngineManager:Can not find any supported ops kernel info store by kernel_name %s,"
        "op type is %s, op name is %s",
        kernel_name.c_str(), op_desc->GetType().c_str(), op_desc->GetName().c_str());
    }
  }
  for (const auto &it : unsupported_reasons) {
    GELOGE(GE_GRAPH_ASSIGN_ENGINE_FAILED, "GetDNNEngineName:Op type %s of ops kernel %s is unsupported, reason:%s",
           op_desc->GetType().c_str(), it.first.c_str(), it.second.c_str());
  }
  GELOGE(GE_GRAPH_ASSIGN_ENGINE_FAILED, "Can't find any supported ops kernel and engine of %s, type is %s",
         op_desc->GetName().c_str(), op_desc->GetType().c_str());
  return "";
}

const std::map<std::string, SchedulerConf> &DNNEngineManager::GetSchedulers() const { return schedulers_; }

Status DNNEngineManager::ParserJsonFile() {
  GELOGI("Begin to parser json file");
  std::string json_file_path = "plugin/nnengine/ge_config/engine_conf.json";
  std::string path = PluginManager::GetPath();
  path.append(json_file_path);
  nlohmann::json scheduler_json_file;
  Status status = ReadJsonFile(path, &scheduler_json_file);
  if (status != SUCCESS) {
    GELOGE(FAILED, "Read scheduler json file failed and the file path is %s", path.c_str());
    return FAILED;
  }
  if (scheduler_json_file.is_null()) {
    // when engine_conf.json is not exist, just return success
    GELOGW("Json file is null");
    return SUCCESS;
  }

  try {
    nlohmann::json scheduler_utils_json = scheduler_json_file[kSchedulerUnits];
    if (scheduler_utils_json.is_null()) {
      GELOGE(FAILED, "The message of scheduler units is not found");
      return FAILED;
    }
    if (!scheduler_utils_json.is_array()) {
      GELOGE(FAILED, "The message of kSchedulerUnits is not array and the file path is %s", json_file_path.c_str());
      return FAILED;
    }
    auto size = scheduler_json_file[kSchedulerUnits].size();
    for (size_t i = 0; i < size; i++) {
      SchedulerConf scheduler_conf;
      std::map<std::string, EngineConfPtr> engine_conf_map;
      nlohmann::json engines_json_map = scheduler_utils_json[i][kCalEngines];
      if (engines_json_map.is_null()) {
        GELOGE(FAILED, "The message of cal_engines is not found");
        return FAILED;
      }
      std::string scheduler_id_temp = scheduler_utils_json[i][kId];
      if (!scheduler_id_temp.empty()) {
        scheduler_conf.id = scheduler_id_temp;
      } else {
        GELOGE(FAILED, "Scheduler ID is null");
        return FAILED;
      }
      status = ParserEngineMessage(engines_json_map, scheduler_id_temp, engine_conf_map);
      if (status != SUCCESS) {
        GELOGE(FAILED, "Parser engines messages failed");
        return FAILED;
      }
      scheduler_conf.name = scheduler_utils_json[i][kName];
      scheduler_conf.ex_attrs = scheduler_utils_json[i][kExAttrs];
      scheduler_conf.cal_engines = engine_conf_map;
      auto it = schedulers_.find(scheduler_id_temp);
      if (it != schedulers_.end()) {
        GELOGE(FAILED, "There are the same scheduler ts %s in the json file", scheduler_id_temp.c_str());
        return FAILED;
      }
      schedulers_.emplace(scheduler_id_temp, scheduler_conf);
    }
  } catch (const nlohmann::detail::type_error &e) {
    GELOGE(FAILED, "Parser json file failed");
    return FAILED;
  }

  GELOGI("Parser json file SUCCESS");
  return SUCCESS;
}

Status DNNEngineManager::ParserEngineMessage(const json engines_json, const std::string &scheduler_mark,
                                             std::map<std::string, EngineConfPtr> &engines) {
  GELOGI("Begin to parser engine massage");
  if (engines_json.is_null()) {
    GELOGE(FAILED, "The message of cal_engines is null");
    return FAILED;
  }
  try {
    if (engines_json.is_array()) {
      for (size_t i = 0; i < engines_json.size(); i++) {
        nlohmann::json engines_elems = engines_json[i];
        EngineConfPtr engine_conf_ptr = MakeShared<EngineConf>();
        if (engine_conf_ptr == nullptr) {
          return FAILED;
        }
        std::string engine_id = engines_elems[kId];
        if (!engine_id.empty()) {
          engine_conf_ptr->id = engine_id;
        } else {
          GELOGE(FAILED, "engineID is null");
          return FAILED;
        }
        if (engines_elems.find(kName) != engines_elems.end()) {
          engine_conf_ptr->name = engines_elems[kName];
        } else {
          GELOGW("The engine %s name is null", engine_id.c_str());
        }
        if (engines_elems.find(kIndependent) != engines_elems.end()) {
          engine_conf_ptr->independent = engines_elems[kIndependent];
        }

        if (engines_elems.find(kAttch) != engines_elems.end()) {
          engine_conf_ptr->attach = engines_elems[kAttch];
        }

        if (engines_elems.find(kSkipAssignStream) != engines_elems.end()) {
          engine_conf_ptr->skip_assign_stream = engines_elems[kSkipAssignStream];
        }
        engine_conf_ptr->scheduler_id = scheduler_mark;
        auto it = engines.find(engine_id);
        if (it != engines.end()) {
          GELOGE(FAILED, "There are the same engine %s message in the json file", engine_id.c_str());
          return FAILED;
        }
        engines.emplace(engine_id, engine_conf_ptr);
      }
    } else {
      GELOGE(FAILED, "The message of cal_engines is not array in the json file");
      return FAILED;
    }
  } catch (const json::exception &e) {
    GELOGE(FAILED, "construct json content failed");
    return FAILED;
  }
  GELOGI("Parser engine massage success");
  return SUCCESS;
}

Status DNNEngineManager::ReadJsonFile(const std::string &file_path, JsonHandle handle) {
  GELOGI("Begin to read json file");
  if (file_path.empty()) {
    GELOGE(FAILED, "Json path %s is not valid", file_path.c_str());
    return FAILED;
  }
  nlohmann::json *json_file = reinterpret_cast<nlohmann::json *>(handle);
  if (json_file == nullptr) {
    GELOGE(FAILED, "JsonFile is nullptr");
    return FAILED;
  }
  const char *file = file_path.data();
  if ((access(file, F_OK)) == -1) {
    if (engines_map_.size() != 0) {
      GELOGE(FAILED, "The json file %s is not exist", file_path.c_str());
      return FAILED;
    } else {
      GELOGW("The json file %s is not need", file_path.c_str());
      return SUCCESS;
    }
  }

  std::ifstream ifs(file_path);
  if (!ifs.is_open()) {
    GELOGE(FAILED, "Open json file %s failed", file_path.c_str());
    return FAILED;
  }

  ifs >> *json_file;
  ifs.close();
  GELOGI("Read json file success");
  return SUCCESS;
}

Status DNNEngineManager::CheckJsonFile() {
  GELOGI("Begin to check json file");
  for (auto &it : engines_map_) {
    std::string engine_name = it.first;
    int count = 0;
    for (auto &iter : schedulers_) {
      auto engine_map = iter.second.cal_engines;
      auto iter_engine_name = engine_map.find(engine_name);
      if (iter_engine_name != engine_map.end()) {
        count++;
      }
    }
    if (count == 0) {
      GELOGE(FAILED, "The engine message %s is not found in the json file", engine_name.c_str());
      return FAILED;
    }
    if (count > 1) {
      GELOGE(FAILED, "The same engine message %s is existed in the json file", engine_name.c_str());
      return FAILED;
    }
  }
  GELOGI("Check json file success");
  return SUCCESS;
}
}  // namespace ge
