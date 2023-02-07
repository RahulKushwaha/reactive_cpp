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

namespace rk::projects::reactive {

template<class T>
class Mono: public Publisher<T> {
 public:
  explicit Mono(T t)
      : payload_{std::move(t)}, state_{
      std::make_unique<State>(State{StateName::SubscriptionStarted})} {}

  void subscribe(std::shared_ptr<Subscriber<T>> subscriber) override {
    std::cout << "Mono Subscribe" << std::endl;
    subscriber_ = std::move(subscriber);

    // Make the state change.
    state_ = std::make_unique<State>(State{StateName::SubscriptionComplete});

    // Create Subscription
    subscription_ = std::make_shared<SubscriptionImpl < T>>
    (*subscriber_.get(), *this);
    // Pass subscription
    subscriber_->onSubscribe(subscription_);
  }

  template<class V>
  std::shared_ptr<Publisher<V>> map(std::function<V(T)> function) {
    std::shared_ptr<Publisher<V>> publisher =
        std::make_shared<MonoMap<T, V>>(function);

    return publisher;
  }

  template<class V>
  std::shared_ptr<Publisher<V>> flatMapIterable() {
    std::shared_ptr<MonoFlatMapIterable<T, V>> publisher =
        std::make_shared<MonoFlatMapIterable<T, V>>();

    auto subscriptionHookLambda = [subscriber = publisher, this]() {
      this->subscribe(subscriber);
    };

    publisher->setSubscriptionHook(subscriptionHookLambda);

    return publisher;
  }

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

  template<class U>
  class SubscriptionImpl: public Subscription {
   public:
    explicit SubscriptionImpl(Subscriber<U> &subscriber,
                              Mono<U> &publisher)
        : subscriber_{subscriber},
          publisher_{publisher} {
    }

    void request(long n) override {
      if (publisher_.state_->stateName != StateName::Complete
          && publisher_.payload_.has_value()) {
        auto &&value = *std::move(publisher_.payload_);

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
    Subscriber<U> &subscriber_;
    Mono<U> &publisher_;
  };


 private:
  std::optional<T> payload_;
  std::shared_ptr<Subscriber<T>> subscriber_;
  std::unique_ptr<State> state_;
  std::shared_ptr<Subscription> subscription_;
};

}


#endif //REACTIVE_CPP_MONO_H
