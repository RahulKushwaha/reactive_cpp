#include <iostream>
#include <vector>
#include "Mono.h"
#include "ConsolePrinterSubscriber.h"
#include <string>

using namespace rk::projects::reactive;

int main() {
  std::cout << "Hello, World!" << std::endl;

  {
    std::cout << "=============Test Block 2=============" << std::endl;
    std::shared_ptr<Mono<std::int32_t>> mon = Mono<std::int32_t>::just(600);

    std::shared_ptr<ConsolePrinterSubscriber>
        consolePrinter = std::make_shared<ConsolePrinterSubscriber>();

    mon->subscribe(consolePrinter);
    std::cout << "======================================" << std::endl;
  }
  {

    std::cout << "=============Test Block 2=============" << std::endl;
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
          if (val < 1000) {
            return true;
          }

          return false;
        })
        ->subscribe(consolePrinter);

    std::cout << "======================================" << std::endl;
  }

  {
    std::cout << "=============Test Block 3=============" << std::endl;
    std::vector<std::int32_t> elements;
    for (int i = 0; i < 59; i++) {
      elements.push_back(i);
    }

    auto flux =
        std::make_shared<FluxJust<std::int32_t>>(std::move(elements));

    std::shared_ptr<ConsolePrinterSubscriber>
        consolePrinter = std::make_shared<ConsolePrinterSubscriber>();

    flux->filter([](auto element) {
          if (element < 10) {
            return true;
          }

          return false;
        })
        ->subscribe(std::move(consolePrinter));
    std::cout << "======================================" << std::endl;
  }

  return 0;
}
