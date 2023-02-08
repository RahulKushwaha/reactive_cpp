//
// Created by Rahul  Kushwaha on 2/8/23.
//

#ifndef REACTIVE_CPP_ABSTRACTMONO_H
#define REACTIVE_CPP_ABSTRACTMONO_H

#include <memory>
#include <utility>
#include <optional>
#include <iostream>

#include "Subscriber.h"

namespace rk::projects::reactive {

template<class A, class B>
class AbstractMono: public Publisher<B>, public Subscriber<A> {
 public:
  explicit AbstractMono(B b)
      : payload_{std::move(b)}, state_{
      std::make_unique<State>(State{StateName::SubscriptionStarted})} {}

  void setSubscriptionHook(std::function<void(void)> func) {
    subscriptionHook_ = std::move(func);
  }

  void subscribe(std::shared_ptr<Subscriber<B>> subscriber) override {
    std::cout << "Mono Subscribe" << std::endl;
    subscriber_ = std::move(subscriber);
    std::cout << "Invoking Hook" << std::endl;
    std::invoke(subscriptionHook_);
    std::cout << "Invoking Hook Complete" << std::endl;
    // Make the state change.
    state_ = std::make_unique<State>(State{StateName::SubscriptionComplete});

    // Create Subscription
    subscription_ = std::make_shared<SubscriptionImpl<A, B>>
        (*subscriber_.get(), *this);
    // Pass subscription
    subscriber_->onSubscribe(subscription_);
  }

  void onSubscribe(std::shared_ptr<Subscription> subscription) override {
    subscription_ = std::move(subscription);
    subscription_->request(1);
  }

  void onNext(A t) override {
    std::cout << "Value Received" << t << std::endl;
  }

  void onError(ReactiveError reactiveError) override {

  }

  void onComplete() override {
  }

  ~AbstractMono() override = default;

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
                              AbstractMono<U, V> &publisher)
        : subscriber_{subscriber},
          publisher_{publisher} {
    }

    void request(long n) override {
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
    AbstractMono<U, V> &publisher_;
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

#endif //REACTIVE_CPP_ABSTRACTMONO_H
