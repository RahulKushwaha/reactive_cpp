#include <iostream>
#include <vector>
#include "Mono.h"
#include "ConsolePrinterSubscriber.h"

using namespace rk::projects::reactive;

int main() {
  std::cout << "Hello, World!" << std::endl;
  {
    std::vector<std::int32_t> elements;
    for (int i = 0; i < 100; i++) {
      elements.push_back(i);
    }

    Mono<std::vector<std::int32_t>>
        vectorMono{std::move(elements)};

    std::shared_ptr<ConsolePrinterSubscriber>
        consolePrinter = std::make_shared<ConsolePrinterSubscriber>();

    vectorMono
        .flatMapIterable<std::int32_t>()
        ->subscribe(consolePrinter);

  }

  return 0;
}
