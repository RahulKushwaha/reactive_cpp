//
// Created by Rahul  Kushwaha on 2/6/23.
//

#pragma once

#include "Subscriber.h"

namespace rk::projects::reactive {

template<class T>
class BaseSubscriber: public Subscriber<T> {
 public:
  BaseSubscriber() : state_{State{OperatorState::Subscribed}} {}

  void onSubscribe(std::shared_ptr<Subscription> subscription) override {
    subscription_ = std::move(subscription);

    subscription_->request(1);
  }

  void onNext(T t) override {
  }

  void onError(ReactiveError reactiveError) override {

  }

  void onComplete() override {
    state_ = State{OperatorState::Completed};
  }

 protected:
  enum class OperatorState {
    Subscribed,
    Completed,
    Errored
  };

  struct State {
    OperatorState operatorState;
  };

  std::shared_ptr<Subscription> subscription_;
  State state_;
};

}
