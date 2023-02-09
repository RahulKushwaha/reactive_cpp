//
// Created by Rahul  Kushwaha on 2/4/23.
//

#pragma once

#include <string>
#include <memory>
#include <vector>

namespace rk::projects::reactive {

template<class T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v) {
  os << "[";
  for (typename std::vector<T>::const_iterator ii = v.begin(); ii != v.end();
       ++ii) {
    os << " " << *ii;
  }
  os << "]";
  return os;
}

class Subscription {
 public:
  virtual void request(long n) = 0;
  virtual void cancel() = 0;

  virtual ~Subscription() = default;
};

class ReactiveError {};

template<class T>
class Subscriber {
 public:
  virtual void onSubscribe(std::shared_ptr<Subscription> subscription) = 0;
  virtual void onNext(T t) = 0;
  virtual void onError(ReactiveError reactiveError) = 0;
  virtual void onComplete() = 0;

  virtual ~Subscriber() = default;
};

template<class T>
class Publisher {
 public:
  virtual void subscribe(std::shared_ptr<Subscriber<T>> subscriber) = 0;

  virtual ~Publisher() = default;
};

}
