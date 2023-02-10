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
class FluxIterable: public Flux<A, B> {
 public:
  void onNext(A t) override {
    for (auto &a: t) {
      Flux<A, B>::q_.push(a);
    }

    std::cout << "Flux: Received a new element: " << t << std::endl;
  }

  ~FluxIterable() override = default;
};

}