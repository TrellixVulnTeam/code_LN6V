/* Copyright 2015 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

// An example Op.

#include "tensorflow/core/framework/common_shape_fns.h"
#include "tensorflow/core/framework/op.h"
#include "tensorflow/core/framework/op_kernel.h"

// OP Register
REGISTER_OP("FactTest")
    .Output("fact: string")
    .SetShapeFn(tensorflow::shape_inference::UnknownShape);

// OP Operation
class FactTestOp : public tensorflow::OpKernel {
 public:
  explicit FactTestOp(tensorflow::OpKernelConstruction* context)
      : OpKernel(context) {}

  void Compute(tensorflow::OpKernelContext* context) override {
    // Output a scalar string.
    tensorflow::Tensor* output_tensor = nullptr;
    OP_REQUIRES_OK(context, context->allocate_output(
                                0, tensorflow::TensorShape(), &output_tensor));
    using tensorflow::string;
    auto output = output_tensor->template scalar<tensorflow::tstring>();

    output() = "0! == 1";
  }
};

// OP Device Bind
REGISTER_KERNEL_BUILDER(Name("FactTest").Device(tensorflow::DEVICE_CPU), FactTestOp);
