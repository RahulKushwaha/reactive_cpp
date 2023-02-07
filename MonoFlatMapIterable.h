//
// Created by Rahul  Kushwaha on 2/6/23.
//

#ifndef REACTIVE_CPP_MONOFLATMAPITERABLE_H
#define REACTIVE_CPP_MONOFLATMAPITERABLE_H

#include "Subscriber.h"

namespace rk::projects::reactive {

template<class T, class V>
class MonoFlatMapIterable: public Publisher<V>, public Subscriber<T> {
 private:
  using val_t = typename T::value_type;
  using vector_val_t = typename std::vector<val_t>;

 public:
  explicit MonoFlatMapIterable() = default;

  void onSubscribe(std::shared_ptr<Subscription> subscription) override {
    std::cout << "MonoFlatMapIterable onSubscribe" << std::endl;
    subscription_ = std::move(subscription);
    subscription_->request(1);
  }

  void onNext(T t) override {
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

  void subscribe(std::shared_ptr<Subscriber<V>> subscriber) override {
    std::cout << "MonoFlatMapIterable Subscribe" << std::endl;
    subscriber_ = std::move(subscriber);

    std::invoke(subscriptionHook_);

    subscription_ = std::make_shared<SubscriptionImpl<T, V>>
        (*subscriber_.get(), *this);

    subscriber_->onSubscribe(subscription_);
  }

  ~MonoFlatMapIterable() override = default;

 private:

  template<class A, class B>
  class SubscriptionImpl: public Subscription {
   public:
    explicit SubscriptionImpl(Subscriber<B> &subscriber,
                              MonoFlatMapIterable<A, B> &publisher)
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
    Subscriber<B> &subscriber_;
    MonoFlatMapIterable<A, B> &publisher_;
    std::int64_t requestedSize_;
    std::int64_t fulfilment_;
  };

  std::shared_ptr<Subscriber<V>> subscriber_;
  std::shared_ptr<Subscription> subscription_;
  T payload_;
  std::function<void(void)> subscriptionHook_;

  std::size_t currentIndex_{0};
};

}

#endif //REACTIVE_CPP_MONOFLATMAPITERABLE_H
