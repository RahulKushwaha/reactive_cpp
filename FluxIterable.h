//
// Created by Rahul  Kushwaha on 2/6/23.
//

#pragma once
#include <iostream>

#include "Flux.h"
#include "Subscriber.h"

namespace rk::projects::reactive {
template<class U, class V>
class Flux;

template<class A, class B = A>
class FluxIterable: public Flux<A, B> {
 private:
  using vector_val_t = typename std::vector<A>;

 public:
  explicit FluxIterable(vector_val_t elements = vector_val_t{}) {
    for (auto &element: elements) {
      auto val = Flux<A, B>::generator_(element);
      Flux<A, B>::q_.push(val);
    }
  }

  void onNext(A t) override {
    for (auto &a: t) {
      Flux<A, B>::q_.push(a);
    }

    std::cout << "Flux: Received a new element: " << t << std::endl;
  }

  ~FluxIterable() override = default;
};

}