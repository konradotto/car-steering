project(helloworld)
add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/helloworld.cpp ${CMAKE_CURRENT_SOURCE_DIR}/PrimeChecker.cpp)

add_executable(${PROJECT_NAME}-Tester TestPrimeChecker.cpp ${CMAKE_CURRENT_SOURCE_DIR}/PrimeChecker.cpp)
add_test(NAME ${PROJECT_NAME}-Tester COMMAND ${PROJECT_NAME}-Tester)