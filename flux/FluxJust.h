//
// Created by Rahul  Kushwaha on 2/6/23.
//

#pragma once
#include <iostream>

#include "Flux.h"
#include "../Subscriber.h"

namespace rk::projects::reactive {
template<class U, class V>
class Flux;

template<class A, class B = A>
class FluxJust: public Flux<A, B> {
 private:
  using vector_val_t = typename std::vector<A>;

 public:
  explicit FluxJust(vector_val_t elements) {
    for (auto &element: elements) {
      Flux<A, B>::q_.push(std::move(element));
    }
  }

  ~FluxJust() override = default;
};

}