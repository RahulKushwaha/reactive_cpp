//
// Created by Rahul  Kushwaha on 2/6/23.
//

#pragma once
#include <iostream>

#include "Flux.h"
#include "Subscriber.h"

namespace rk::projects::reactive {
template<class U, class V>
class Flux;

template<class A, class B = A>
class FluxJust: public Flux<A, B> {
 private:
  using vector_val_t = typename std::vector<A>;

 public:
  explicit FluxJust(vector_val_t elements)
      : payload_{std::move(elements)} {}

  void subscribe(std::shared_ptr<Subscriber<B>> subscriber) override {
    std::cout << "FluxJust Subscribe" << std::endl;
    Flux<A, B>::subscriber_ = std::move(subscriber);

    std::invoke(Flux<A, B>::subscriptionHook_);

    Flux<A, B>::subscription_ = std::make_shared<SubscriptionImpl < A, B>>
    (*Flux<A, B>::subscriber_.get(), *this);

    Flux<A, B>::subscriber_->onSubscribe(Flux<A, B>::subscription_);
  }

  ~ FluxJust() override = default;

 private:

  template<class U, class V>
  class SubscriptionImpl: public Subscription {
   public:
    explicit SubscriptionImpl(Subscriber<V> &subscriber,
                              FluxJust<U, V> &publisher)
        : subscriber_{subscriber},
          publisher_{publisher},
          requestedSize_{0},
          fulfilment_{0} {
    }

    void request(long n) override {
      requestedSize_ += n;

      if (fulfilment_ == 0) {
        auto &vectorElements = static_cast<vector_val_t &>(publisher_.payload_);

        while (requestedSize_ > 0
            && publisher_.currentIndex_ < vectorElements.size()) {
          fulfilment_ = requestedSize_;
          requestedSize_ = 0;

          std::int64_t iteration = 0;
          while (iteration < fulfilment_
              && publisher_.currentIndex_ < vectorElements.size()) {
            auto resultIndex = publisher_.currentIndex_;

            publisher_.currentIndex_++;
            iteration++;

            subscriber_.onNext(vectorElements[resultIndex]);
          }

          if (publisher_.currentIndex_ >= vectorElements.size() - 1) {
            subscriber_.onComplete();
          }

          fulfilment_ = 0;
        }
      }
    }

    void cancel() override {
    }

    ~SubscriptionImpl() override = default;

   private:
    Subscriber<V> &subscriber_;
    FluxJust<U, V> &publisher_;
    std::int64_t requestedSize_;
    std::int64_t fulfilment_;
  };

 private:
  std::size_t currentIndex_{0};
  vector_val_t payload_;
};

}