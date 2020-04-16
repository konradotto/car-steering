cmake_minimum_required(VERSION 3.2.2)
project(cyberGroup13)

add_subdirectory(helloworld)
add_subdirectory(image-cropping)

enable_testing()
add_executable(${PROJECT_NAME}-Runner helloworld/TestPrimeChecker.cpp ${CMAKE_CURRENT_SOURCE_DIR}/helloworld/PrimeChecker.cpp)
add_test(NAME ${PROJECT_NAME}-Runner COMMAND ${PROJECT_NAME}-Runner)