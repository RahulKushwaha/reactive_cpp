//
// Created by Rahul  Kushwaha on 2/8/23.
//

#pragma once

#include <functional>
#include <memory>

#include "MonoJust.h"
#include "MonoFilter.h"
#include "MonoMap.h"
#include "../Subscriber.h"
#include "../flux/FluxJust.h"
#include "../flux/FluxIterable.h"
#include "MonoFlatMap.h"

namespace rk::projects::reactive {

template<class A, class B>
class Flux;

template<class A, class B = A>
class Mono: public Publisher<B>, public Subscriber<A> {
 public:
  explicit Mono(B b) : payload_{b} {}
  Mono() : payload_{} {}

  static std::shared_ptr<Mono<A, B>> just(B a) {
    std::shared_ptr<MonoJust<B>> publisher =
        std::make_shared<MonoJust<B>>(std::move(a));

    return publisher;
  }

  std::shared_ptr<Mono<A, A>> filter(std::function<bool(A)> func) {
    std::shared_ptr<MonoFilter<A>> publisher =
        std::make_shared<MonoFilter<A>>(std::move(func));

    auto subscriptionHookLambda = [subscriber = publisher, this]() {
      this->subscribe(subscriber);
    };

    publisher->setSubscriptionHook(subscriptionHookLambda);

    return publisher;
  }

  template<class V>
  std::shared_ptr<Mono<B, V>> map(std::function<V(B)> functor) {
    std::shared_ptr<MonoMap<B, V>> publisher =
        std::make_shared<MonoMap<B, V>>(std::move(functor));

    auto subscriptionHookLambda = [subscriber = publisher, this]() {
      this->subscribe(subscriber);
    };

    publisher->setSubscriptionHook(subscriptionHookLambda);

    return publisher;
  }

  template<class V>
  std::shared_ptr<Mono<B, V>>
  flatMap(std::function<std::shared_ptr<Mono<V, V>>(B)> func) {
    std::shared_ptr<MonoFlatMap<B, V >> publisher =
        std::make_shared<MonoFlatMap<B, V>>(std::move(func));

    auto subscriptionHookLambda = [subscriber = publisher, this]() {
      this->subscribe(subscriber);
    };

    publisher->setSubscriptionHook(subscriptionHookLambda);

    return publisher->inner();
  }


  template<class V>
  std::shared_ptr<Flux<B, V>> flatMapIterable() {
    std::shared_ptr<FluxIterable<B, V >> publisher =
        std::make_shared<FluxIterable<B, V>>();

    auto subscriptionHookLambda = [subscriber = publisher, this]() {
      this->subscribe(subscriber);
    };

    publisher->setSubscriptionHook(subscriptionHookLambda);

    return publisher;
  }

  void setSubscriptionHook(std::function<void(void)> func) {
    subscriptionHook_ = std::move(func);
  }

  void subscribe(std::shared_ptr<Subscriber<B>> subscriber) override {
    std::cout << "Mono Subscribe" << std::endl;

    subscriber_ = std::move(subscriber);
    std::invoke(subscriptionHook_);

    // Make the state change.
    state_ = std::make_unique<State>(State{StateName::SubscriptionComplete});

    // Create Subscription
    subscription_ = std::make_shared<MonoSubscription>
        (*subscriber_.get(), *this);
    // Pass subscription
    subscriber_->onSubscribe(subscription_);
  }

  void onSubscribe(std::shared_ptr<Subscription> subscription) override {
    subscription_ = std::move(subscription);
    subscription_->request(1);
  }

  void onError(ReactiveError reactiveError) override {

  }

  void onComplete() override {
  }

  ~Mono() override = default;

 private:
  enum class StateName {
    SubscriptionStarted,
    SubscriptionComplete,
    Processing,
    Error,
    Complete,
  };

  struct State {
    StateName stateName;
  };

  class MonoSubscription;

 protected:
  std::optional<B> payload_;
  std::shared_ptr<Subscriber<B>> subscriber_;
  std::unique_ptr<State> state_;
  std::shared_ptr<Subscription> subscription_;
  // Initialize with empty function
  std::function<void(void)> subscriptionHook_{[]() {}};
};

template<class U, class V>
class Mono<U, V>::MonoSubscription: public Subscription {
 public:
  explicit MonoSubscription(Subscriber<V> &subscriber,
                            Mono<U, V> &publisher)
      : subscriber_{subscriber},
        publisher_{publisher} {
  }

  void request(long n) override {
    std::cout << "Mono Request" << std::endl;
    if (publisher_.state_->stateName != StateName::Complete
        && publisher_.payload_.has_value()) {
      // Fix me: Move object
      auto value = *publisher_.payload_;
      publisher_.payload_.reset();
      subscriber_.onNext(std::move(value));


      publisher_.state_ =
          std::make_unique<State>(State{StateName::Complete});
    }

    subscriber_.onComplete();
  }

  void cancel() override {
  }

  ~MonoSubscription() override = default;

 private:
  Subscriber<V> &subscriber_;
  Mono<U, V> &publisher_;
};

}
