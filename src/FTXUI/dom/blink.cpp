// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <memory>   // for make_shared
#include <utility>  // for move

#include "elements.hpp"        // for Element, blink
#include "node.hpp"            // for Node
#include "node_decorator.hpp"  // for NodeDecorator
#include "../screen/box.hpp"          // for Box
#include "../screen/screen.hpp"       // for Pixel, Screen

namespace ftxui {

namespace {
class Blink : public NodeDecorator {
 public:
  using NodeDecorator::NodeDecorator;

  void Render(Screen& screen) override {
    Node::Render(screen);
    for (int y = box_.y_min; y <= box_.y_max; ++y) {
      for (int x = box_.x_min; x <= box_.x_max; ++x) {
        screen.PixelAt(x, y).blink = true;
      }
    }
  }
};
}  // namespace

/// @brief The text drawn alternates in between visible and hidden.
/// @ingroup dom
Element blink(Element child) {
  return std::make_shared<Blink>(std::move(child));
}

}  // namespace ftxui
