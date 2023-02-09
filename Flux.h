//
// Created by Rahul  Kushwaha on 2/8/23.
//

#pragma once
#include "Subscriber.h"
#include "FluxFilter.h"

#include <iostream>
#include <queue>

namespace rk::projects::reactive {

template<class A, class B = A>
class Flux: public Subscriber<A>, public Publisher<B> {
 public:
  explicit Flux()
      : state_{
      std::make_unique<State>(State{StateName::SubscriptionStarted})},
        functor_{[this](A a) {
          if (q_.empty()) {
            return B{};
          }

          auto val = q_.front();
          q_.pop();
          return val;
        }},
        pusher_{[this](A a) {
          auto val = functor_(a);
          q_.push(val);
        }} {
  };

  std::shared_ptr<FluxFilter<A>> filter(std::function<bool(A)> func) {
    std::shared_ptr<FluxFilter<A>> publisher =
        std::make_shared<FluxFilter<A>>(std::move(func));

    auto subscriptionHookLambda = [subscriber = publisher, this]() {
      this->subscribe(subscriber);
    };

    publisher->setSubscriptionHook(subscriptionHookLambda);

    return publisher;
  }

  void onSubscribe(std::shared_ptr<Subscription> subscription) override {
    std::cout << "MonoFlatMapIterable onSubscribe" << std::endl;
    subscription_ = std::move(subscription);
    subscription_->request(1);
  }

  void onNext(A t) override {
    pusher_(t);
    std::cout << "Flux: Received a new element: " << t << std::endl;

    if (state_->stateName != StateName::Complete) {
      subscription_->request(1);
    }
  }

  void onError(ReactiveError reactiveError) override {
    subscriber_->onError(reactiveError);
  }

  void onComplete() override {
    state_ = std::make_unique<State>(State{StateName::Complete});
  }

  void setSubscriptionHook(std::function<void(void)> func) {
    subscriptionHook_ = std::move(func);
  }

  void subscribe(std::shared_ptr<Subscriber<B>> subscriber) override {
    std::cout << "Flux Subscribe" << std::endl;
    subscriber_ = std::move(subscriber);

    std::invoke(subscriptionHook_);

    subscription_ = std::make_shared<SubscriptionImpl < A, B>>
    (*subscriber_.get(), *this);

    subscriber_->onSubscribe(subscription_);
  }

  ~Flux() override = default;

 private:

  template<class U, class V>
  class SubscriptionImpl:
      public Subscription {
   public:
    explicit SubscriptionImpl(Subscriber<V>
                              &subscriber,
                              Flux<U, V> &publisher)
        : subscriber_{subscriber},
          publisher_{publisher},
          requestedSize_{0},
          fulfilment_{0} {
    }

    void request(long n)
    override {
      requestedSize_ += n;

      if (fulfilment_ == 0) {
        while (requestedSize_ > 0 && !publisher_.q_.empty()) {
          fulfilment_ = requestedSize_;
          requestedSize_ = 0;

          std::int64_t iteration = 0;
          while (iteration < fulfilment_ && !publisher_.q_.empty()) {

            iteration++;

            auto top = publisher_.functor_(A{});

            subscriber_.onNext(top);
          }

          if (publisher_.q_.empty()) {
            subscriber_.onComplete();
          }

          fulfilment_ = 0;
        }
      }
    }

    void cancel()
    override {
    }

    ~SubscriptionImpl()
    override =
    default;

   private:
    Subscriber<V> &subscriber_;
    Flux<U, V> &publisher_;
    std::int64_t requestedSize_;
    std::int64_t fulfilment_;
  };

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

  std::unique_ptr<State> state_;


 protected:
  std::shared_ptr<Subscriber<B>> subscriber_;
  std::shared_ptr<Subscription> subscription_;
  std::queue<B> q_;
  std::function<void(void)> subscriptionHook_{[]() {}};
  std::function<B(A)> functor_;
  std::function<void(A)> pusher_;
};

}
