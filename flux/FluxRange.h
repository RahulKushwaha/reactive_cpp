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
class FluxRange: public Flux<A, B> {
 public:
  explicit FluxRange(std::int64_t start = 0, std::int64_t end = 0)
      : start_{start}, end_{end} {
    Flux<A, B>::generator_ = [this](A a) mutable {
      auto returnValue = start_++;
      return returnValue;
    };

    Flux<A, B>::terminationCondition_ = [this]() {
      return start_ >= end_;
    };
  }

  ~FluxRange() override = default;

 private:
  std::int64_t start_;
  std::int64_t end_;
};

}