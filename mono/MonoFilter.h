//
// Created by Rahul  Kushwaha on 2/7/23.
//
#pragma once

#include "../Subscriber.h"

namespace rk::projects::reactive {

template<class U, class V>
class Mono;

template<class A>
class MonoFilter: public virtual Mono<A, A> {
 public:
  explicit MonoFilter(std::function<bool(A)> functor)
      : functor_{std::move(functor)} {}

  void onNext(A a) override {
    std::cout << "Value Received: " << a << std::endl;
    if (functor_(a)) {
      Mono<A, A>::payload_ = a;
    } else {
      Mono<A, A>::payload_.reset();
    }
  }

  ~MonoFilter() override = default;

 private:
  std::function<bool(A)> functor_;
};

}
