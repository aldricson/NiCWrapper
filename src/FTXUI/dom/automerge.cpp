// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <memory>   // for make_shared
#include <utility>  // for move

#include "elements.hpp"        // for Element, automerge
#include "node.hpp"            // for Node
#include "node_decorator.hpp"  // for NodeDecorator
#include "../screen/box.hpp"          // for Box
#include "../screen/screen.hpp"       // for Pixel, Screen

namespace ftxui {

/// @brief Enable character to be automatically merged with others nearby.
/// @ingroup dom
Element automerge(Element child) {
  class Impl : public NodeDecorator {
   public:
    using NodeDecorator::NodeDecorator;

    void Render(Screen& screen) override {
      for (int y = box_.y_min; y <= box_.y_max; ++y) {
        for (int x = box_.x_min; x <= box_.x_max; ++x) {
          screen.PixelAt(x, y).automerge = true;
        }
      }
      Node::Render(screen);
    }
  };

  return std::make_shared<Impl>(std::move(child));
}

}  // namespace ftxui
