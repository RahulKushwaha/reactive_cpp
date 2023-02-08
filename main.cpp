#include <iostream>
#include <vector>
#include "MonoJust.h"
#include "Mono.h"
#include "ConsolePrinterSubscriber.h"
#include <string>

using namespace rk::projects::reactive;

int main() {
  std::cout << "Hello, World!" << std::endl;

  {
//    std::shared_ptr<Mono_<std::int32_t>>
//        mon = Mono_<std::int32_t>::just(600);
//
//    std::shared_ptr<ConsolePrinterSubscriber>
//        consolePrinter = std::make_shared<ConsolePrinterSubscriber>();
//
//    mon->subscribe(consolePrinter);
  }
  {
    std::shared_ptr<ConsolePrinterSubscriber>
        consolePrinter = std::make_shared<ConsolePrinterSubscriber>();


    std::string input = "90";
    std::shared_ptr<Mono<std::string>>
        mon = Mono<std::string>::just(input);

    mon->template map<int>([](const std::string &input) {
          return std::stoi(input);
        })
        ->
            template map<int>([](int in) {
          return in * 8;
        })
        ->filter([](auto val) {
          return true;
        })
        ->subscribe(consolePrinter);
  }

  {
//    Mono<std::string> monoString{"799"};
//    auto stringToIntMono = monoString.template map<std::int32_t>(
//        [](const std::string &input)
//            -> std::int32_t { return std::stoi(input); });
//
//    std::shared_ptr<ConsolePrinterSubscriber>
//        consolePrinter = std::make_shared<ConsolePrinterSubscriber>();
//
//    stringToIntMono->subscribe(consolePrinter);
  }
  {
//    std::vector<std::int32_t> elements;
//    for (int i = 0; i < 100; i++) {
//      elements.push_back(i);
//    }
//
//    Mono<std::vector<std::int32_t>>
//        vectorMono{std::move(elements)};
//
//    std::shared_ptr<ConsolePrinterSubscriber>
//        consolePrinter = std::make_shared<ConsolePrinterSubscriber>();
//
//    vectorMono
//        .flatMapIterable<std::int32_t>()
//        ->subscribe(consolePrinter);

  }

  return 0;
}
