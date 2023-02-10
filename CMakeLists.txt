cmake_minimum_required(VERSION 3.24)
project(reactive_cpp)

set(CMAKE_CXX_STANDARD 20)

add_executable(reactive_cpp main.cpp Subscriber.h mono/MonoJust.h BaseSubscriber.h
        ConsolePrinterSubscriber.h flux/FluxJust.h mono/MonoMap.h mono/MonoFilter.h
        mono/Mono.h flux/Flux.h flux/FluxFilter.h mono/MonoFlatMap.h flux/FluxIterable.h flux/FluxRange.h)

target_link_libraries(reactive_cpp -fsanitize=address)
