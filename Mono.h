//
// Created by Rahul  Kushwaha on 2/6/23.
//

#ifndef REACTIVE_CPP_MONO_H
#define REACTIVE_CPP_MONO_H
#include <unordered_map>
#include <memory>

#include "Subscriber.h"
#include "MonoMap.h"
#include "MonoFlatMapIterable.h"
#include "AbstractMono.h"

namespace rk::projects::reactive {

template<class A, class B = A>
class Mono: public AbstractMono<A, B> {
 public:
  explicit Mono(A t)
      : AbstractMono<A, B>(std::move(t)) {}

  template<class V>
  std::shared_ptr<Publisher<V>> flatMapIterable() {
    std::shared_ptr<MonoFlatMapIterable<A, V>> publisher =
        std::make_shared<MonoFlatMapIterable<A, V>>();

    auto subscriptionHookLambda = [subscriber = publisher, this]() {
      this->subscribe(subscriber);
    };

    publisher->setSubscriptionHook(subscriptionHookLambda);

    return publisher;
  }

  template<class V>
  std::shared_ptr<Publisher<V>> map(std::function<V(B)> functor) {
    std::shared_ptr<MonoMap<B, V>> publisher =
        std::make_shared<MonoMap<B, V>>(std::move(functor));

    auto subscriptionHookLambda = [subscriber = publisher, this]() {
      this->subscribe(subscriber);
    };

    publisher->setSubscriptionHook(subscriptionHookLambda);

    return publisher;
  }

  ~Mono() override = default;
};

}


#endif //REACTIVE_CPP_MONO_H
