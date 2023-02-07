//
// Created by Rahul  Kushwaha on 2/6/23.
//

#ifndef REACTIVE_CPP_CONSOLEPRINTERSUBSCRIBER_H
#define REACTIVE_CPP_CONSOLEPRINTERSUBSCRIBER_H
#include <cstdint>
#include <iostream>

#include "BaseSubscriber.h"

namespace rk::projects::reactive {

class ConsolePrinterSubscriber: public BaseSubscriber<std::int32_t> {
 public:
  void onNext(std::int32_t num) override {

    std::cout << "Number Consumed: " << num << std::endl;

    // Call base class for state change.
    BaseSubscriber::onNext(num);

    if (BaseSubscriber::state_.operatorState
        != BaseSubscriber::OperatorState::Completed) {
      BaseSubscriber::subscription_->request(1);
    }
  }

  void onSubscribe(std::shared_ptr<Subscription> subscription) override {
    std::cout << "ConsolePrinterSubscriber onSubscribe" << std::endl;
    BaseSubscriber::onSubscribe(std::move(subscription));
  }

  ~ConsolePrinterSubscriber() override = default;
};

}


#endif //REACTIVE_CPP_CONSOLEPRINTERSUBSCRIBER_H