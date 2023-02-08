//
// Created by Rahul  Kushwaha on 2/8/23.
//

#pragma once

#include <functional>
#include <memory>

#include "MonoJust.h"
#include "MonoFilter.h"
#include "MonoMap.h"
#include "Subscriber.h"

namespace rk::projects::reactive {

template<class A, class B = A>
class Mono: public Publisher<B>, public Subscriber<A> {
 public:
  explicit Mono(B b) : payload_{b} {}

  static std::shared_ptr<Mono<A, B>> just(B a) {
    std::shared_ptr<MonoJust<B>> publisher =
        std::make_shared<MonoJust<B>>(std::move(a));

    return publisher;
  }

  std::shared_ptr<Mono<A, A>> filter(std::function<bool(A)> func) {
    std::shared_ptr<MonoFilter<A>>
        publisher =
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
    subscription_ = std::make_shared<SubscriptionImpl < A, B>>
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

  template<class U, class V>
  class SubscriptionImpl: public Subscription {
   public:
    explicit SubscriptionImpl(Subscriber<V> &subscriber,
                              Mono<U, V> &publisher)
        : subscriber_{subscriber},
          publisher_{publisher} {
    }

    void request(long n) override {
      std::cout << "AbstractMono Request" << std::endl;
      if (publisher_.state_->stateName != StateName::Complete
          && publisher_.payload_.has_value()) {
        auto &&value = *std::move(publisher_.payload_);
        publisher_.payload_.reset();
        subscriber_.onNext(std::move(value));


        publisher_.state_ =
            std::make_unique<State>(State{StateName::Complete});
      }

      subscriber_.onComplete();
    }

    void cancel() override {
    }

    ~SubscriptionImpl() override = default;

   private:
    Subscriber<V> &subscriber_;
    Mono<U, V> &publisher_;
  };


 protected:
  std::optional<B> payload_;
  std::shared_ptr<Subscriber<B>> subscriber_;
  std::unique_ptr<State> state_;
  std::shared_ptr<Subscription> subscription_;
  // Initialize with empty function
  std::function<void(void)> subscriptionHook_{[]() {}};
};

}
