// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include "node.hpp"  // for Node, Elements
#include <memory>              // for __shared_ptr_access
#include <vector>              // for __alloc_traits<>::value_type

#include "node_decorator.hpp"
#include "requirement.hpp"  // for Requirement
#include "../screen/box.hpp"       // for Box

namespace ftxui {

void NodeDecorator::ComputeRequirement() {
  Node::ComputeRequirement();
  requirement_ = children_[0]->requirement();
}

void NodeDecorator::SetBox(Box box) {
  Node::SetBox(box);
  children_[0]->SetBox(box);
}

}  // namespace ftxui