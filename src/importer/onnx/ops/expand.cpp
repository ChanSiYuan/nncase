/* Copyright 2020 Alexey Chernov <4ernov@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../onnx_importer.h"
#include <cassert>
#include <nncase/ir/graph.h>
#include <nncase/ir/ops/binary.h>
#include <nncase/ir/ops/constant.h>
#include <xtensor/xadapt.hpp>

using namespace nncase;
using namespace nncase::importer;
using namespace nncase::ir;
using namespace onnx;

void onnx_importer::convert_op_Expand(const NodeProto &node)
{
    assert(node.input().size() == 2);
    const auto &input = node.input()[0];
    const auto &shape_name = node.input()[1];
    const auto &output = node.output()[0];

    const auto input_type = get_datatype(input).value();
    const auto &input_shape = get_shape(input);

    shape_t shape;
    const auto &shape_initializer = get_initializer(shape_name);
    if (shape_initializer)
    {
        auto shape_value = to<axis_t>(shape_initializer.value());
        std::transform(std::begin(shape_value), std::end(shape_value), std::back_inserter(shape),
            [](const auto axis) { return static_cast<size_t>(axis); });
    }
    else
    {
        const auto data = get_constant_input_data<int64_t>(shape_name);
        if (data)
            std::transform(std::begin(data.value()), std::end(data.value()), std::back_inserter(shape),
                [](const auto axis) { return static_cast<size_t>(axis); });
    }

    auto ones = xt::ones<float>(shape);
    std::vector<float> vec { ones.begin(), ones.end() };
    auto con = graph_.emplace<constant>(input_type, shape, vec);
    auto op = graph_.emplace<binary>(binary_mul, input_shape, shape, value_range<float>::full());
    op->name(generate_name(node) + "(Expand)");

    op->input_b().connect(con->output());
    input_tensors_.emplace(&op->input_a(), input);
    output_tensors_.emplace(output, &op->output());
}