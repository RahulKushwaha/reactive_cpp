#include <iostream>
#include <vector>
#include "Mono.h"
#include "ConsolePrinterSubscriber.h"
#include <string>

using namespace rk::projects::reactive;

int main() {
  std::cout << "Hello, World!" << std::endl;

  {
    Mono<std::string> monoString{"7"};
    auto stringToIntMono = monoString.template map<std::int32_t>(
        [](std::string input)
            -> std::int32_t { return 7; });

    std::shared_ptr<ConsolePrinterSubscriber>
        consolePrinter = std::make_shared<ConsolePrinterSubscriber>();

    stringToIntMono->subscribe(consolePrinter);
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
