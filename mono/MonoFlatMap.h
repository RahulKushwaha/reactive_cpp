//
// Created by Rahul  Kushwaha on 2/9/23.
//
#pragma once
#include <iostream>
#include <functional>
#include <optional>

namespace rk::projects::reactive {
template<class U, class V>
class Mono;

template<class A, class B>
class MonoFlatMap: public Mono<A, B> {
 public:
  explicit MonoFlatMap(
      std::function<std::shared_ptr<Mono<B, B>>(A)> functor)
      : innerMono_{std::move(functor)(B{})} {}

  void onNext(A a) override {
    std::cout << "Mono Flat Map Value Received: " << a << std::endl;
    this->subscribe(innerMono_);
  }

  std::shared_ptr<Mono<B, B>> inner() {
    return innerMono_;
  }

 private:
  std::shared_ptr<Mono<B, B>> innerMono_;
};

}
