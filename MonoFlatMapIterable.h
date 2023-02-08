//
// Created by Rahul  Kushwaha on 2/6/23.
//

#pragma once

#include "Subscriber.h"

namespace rk::projects::reactive {

template<class A, class B>
class MonoFlatMapIterable: public Publisher<B>, public Subscriber<A> {
 private:
  using val_t = typename A::value_type;
  using vector_val_t = typename std::vector<val_t>;

 public:
  explicit MonoFlatMapIterable() = default;

  void onSubscribe(std::shared_ptr<Subscription> subscription) override {
    std::cout << "MonoFlatMapIterable onSubscribe" << std::endl;
    subscription_ = std::move(subscription);
    subscription_->request(1);
  }

  void onNext(A t) override {
    payload_ = std::move(t);

    auto &vectorElements = static_cast<vector_val_t &>(payload_);
    std::cout << "Received Vector of Elements: " << vectorElements.size()
              << std::endl;
  }

  void onError(ReactiveError reactiveError) override {
    subscriber_->onError(reactiveError);
  }

  void onComplete() override {
  }

  void setSubscriptionHook(std::function<void(void)> func) {
    subscriptionHook_ = std::move(func);
  }

  void subscribe(std::shared_ptr<Subscriber<B>> subscriber) override {
    std::cout << "MonoFlatMapIterable Subscribe" << std::endl;
    subscriber_ = std::move(subscriber);

    std::invoke(subscriptionHook_);

    subscription_ = std::make_shared<SubscriptionImpl < A, B>>
    (*subscriber_.get(), *this);

    subscriber_->onSubscribe(subscription_);
  }

  ~MonoFlatMapIterable() override = default;

 private:

  template<class U, class V>
  class SubscriptionImpl: public Subscription {
   public:
    explicit SubscriptionImpl(Subscriber<V> &subscriber,
                              MonoFlatMapIterable<U, V> &publisher)
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
    MonoFlatMapIterable<U, V> &publisher_;
    std::int64_t requestedSize_;
    std::int64_t fulfilment_;
  };

  std::shared_ptr<Subscriber<B>> subscriber_;
  std::shared_ptr<Subscription> subscription_;
  A payload_;
  std::function<void(void)> subscriptionHook_;

  std::size_t currentIndex_{0};
};

}