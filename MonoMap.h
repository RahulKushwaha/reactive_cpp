//
// Created by Rahul  Kushwaha on 2/7/23.
//

#ifndef REACTIVE_CPP_MONOMAP_H
#define REACTIVE_CPP_MONOMAP_H
#include "Subscriber.h"

namespace rk::projects::reactive {

template<class T, class V>
class MonoMap: public Subscriber<T>, public Publisher<V> {
 public:
  template<class A, class B>
  static MonoMap<A, B> makeMonoMap(std::function<B(A)> func) {
    auto monoMap = MonoMap<A, B>{};
    monoMap.functor_ = std::move(func);
  }

  void onSubscribe(std::shared_ptr<Subscription> subscription) override {

  }

  void onNext(T t) override {

  }

  void onError(ReactiveError reactiveError) override {

  }

  void onComplete() override {

  }

  void subscribe(std::shared_ptr<Subscriber<T>> subscriber) {
    subscriber_ = subscriber;
  }

  ~MonoMap() override = default;

 private:
  std::shared_ptr<Subscriber<T>> subscriber_;
  std::function<V(T)> functor_;
};

}
#endif //REACTIVE_CPP_MONOMAP_H
