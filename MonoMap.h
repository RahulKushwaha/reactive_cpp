//
// Created by Rahul  Kushwaha on 2/7/23.
//

#ifndef REACTIVE_CPP_MONOMAP_H
#define REACTIVE_CPP_MONOMAP_H
#include "Subscriber.h"
#include "AbstractMono.h"

namespace rk::projects::reactive {

template<class A, class B>
class MonoMap: public AbstractMono<A, B> {
 public:
  explicit MonoMap(std::function<B(A)> functor)
      : AbstractMono<A, B>({}),
        functor_{std::move(functor)} {}

  void onNext(A a) override {
    std::cout << "Value Received: " << a << std::endl;
    AbstractMono<A, B>::payload_ = functor_(a);
  }

  ~MonoMap() override = default;

 private:
  std::function<B(A)> functor_;
};

}
#endif //REACTIVE_CPP_MONOMAP_H
