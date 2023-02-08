//
// Created by Rahul  Kushwaha on 2/7/23.
//
#pragma once

namespace rk::projects::reactive {
template<class U, class V>
class Mono;

template<class A, class B>
class MonoMap: public Mono<A, B> {
 public:
  explicit MonoMap(std::function<B(A)> functor)
      : Mono<A, B>{B{}}, functor_{std::move(functor)} {}

  void onNext(A a) override {
    std::cout << "Value Received: " << a << std::endl;
    Mono<A, B>::payload_ = functor_(a);
  }

  ~MonoMap() override = default;

 private:
  std::function<B(A)> functor_;
};

}

