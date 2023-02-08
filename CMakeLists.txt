cmake_minimum_required(VERSION 3.24)
project(reactive_cpp)

set(CMAKE_CXX_STANDARD 20)

add_executable(reactive_cpp main.cpp Subscriber.h MonoJust.h BaseSubscriber.h
        ConsolePrinterSubscriber.h MonoFlatMapIterable.h MonoMap.h MonoFilter.h Mono.h)

target_link_libraries(reactive_cpp -fsanitize=address)
