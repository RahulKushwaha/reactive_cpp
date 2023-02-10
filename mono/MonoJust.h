//
// Created by Rahul  Kushwaha on 2/6/23.
//
#pragma once
#include <unordered_map>
#include <memory>

#include "../Subscriber.h"

namespace rk::projects::reactive {
template<class U, class V>
class Mono;

template<class A, class B = A>
class MonoJust: public Mono<A, B> {
 public:
  explicit MonoJust(A t) : Mono<A, B>(std::move(t)) {}

  void onNext(A t) override {
    Mono<A, B>::payload_ = {t};
  }

  ~MonoJust() override = default;
};

}
