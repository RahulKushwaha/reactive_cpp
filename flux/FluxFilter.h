//
// Created by Rahul  Kushwaha on 2/8/23.
//

#pragma once
#include <iostream>

#include "Flux.h"
#include "../Subscriber.h"

namespace rk::projects::reactive {
template<class U, class V>
class Flux;

template<class A>
class FluxFilter: public Flux<A, A> {
 public:
  explicit FluxFilter(std::function<bool(A)> functor)
      : functor_{std::move(functor)} {}

  void onNext(A a) override {
    if (functor_(a)) {
      Flux<A, A>::onNext(a);
    } else {
      std::cout << "Element Filtered" << std::endl;
    }
  }

  ~FluxFilter() override = default;

 private:


  std::function<bool(A)> functor_;
};

}