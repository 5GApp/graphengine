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

#ifndef GE_GRAPH_PREPROCESS_INSERT_OP_BASE_INSERT_OP_H_
#define GE_GRAPH_PREPROCESS_INSERT_OP_BASE_INSERT_OP_H_

#include <string>
#include <utility>
#include <vector>
#include <memory>

#include "common/fmk_error_codes.h"
#include "common/types.h"
#include "framework/common/ge_inner_error_codes.h"
#include "graph/compute_graph.h"
#include "proto/om.pb.h"
#include "proto/insert_op.pb.h"

namespace ge {
class InsertOpBase {
 public:
  virtual ~InsertOpBase() = default;

  ///
  /// @ingroup ge_omg
  /// @brief Configure the default insertop parameter
  ///
  virtual Status SetDefaultParams() = 0;

  ///
  /// @ingroup ge_omg
  /// @brief Verify the insertop parameter
  ///
  virtual Status ValidateParams() = 0;

  ///
  /// @ingroup ge_omg
  /// @brief Insert the insert_op operator into the network graph
  /// @param [in] graph
  ///
  virtual Status InsertOpToGraph(ge::ComputeGraphPtr graph);

  ///
  /// @ingroup ge_omg
  /// @brief Insert aipp operator into the network graph
  /// @param [in] graph
  /// @param [in] aippConfigPath aipp
  ///
  virtual Status InsertAippToGraph(ge::ComputeGraphPtr &graph, std::string &aippConfigPath,
                                   ge::NodePtr &inserted_aipp_node);

  ///
  /// @ingroup ge_omg
  /// @brief get aipp mode : static or dyanmic
  /// @param [in] aipp node
  ///
  virtual domi::AippOpParams::AippMode GetAippMode() = 0;

 protected:
  ///
  /// @ingroup ge_omg
  /// @brief Get the input operator in the model
  ///
  static Status GetInputNode(ge::ComputeGraphPtr graph, ge::NodePtr &target_input, uint32_t rank);

  ///
  /// @ingroup ge_omg
  /// @brief Get the size of data bases on the input
  ///
  uint32_t AdjustDataSize(const ge::GeTensorDesc &desc, std::unique_ptr<domi::AippOpParams> &aipp_params);

  ///
  /// @ingroup ge_omg
  /// @brief Generate the insert_op operator
  ///
  virtual Status GenerateOpDesc(ge::OpDescPtr op_desc) = 0;

  ///
  /// @ingroup ge_omg
  /// @brief Get the target operator
  /// @param [in] graph graph
  /// @param [in|out] target_input target operator
  /// @param [in|out] target_edges target edge
  ///
  virtual Status GetTargetPosition(ge::ComputeGraphPtr graph, ge::NodePtr &target_input,
                                   std::vector<std::pair<ge::OutDataAnchorPtr, ge::InDataAnchorPtr>> &target_edges) = 0;
};
}  // namespace ge

#endif  // GE_GRAPH_PREPROCESS_INSERT_OP_BASE_INSERT_OP_H_

