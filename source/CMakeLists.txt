# Copyright (C) 2020  Christian Berger
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
project(cyberGroup13)


cmake_minimum_required(VERSION 3.2.2)

enable_testing()

################################################################################
# Defining the relevant versions of OpenDLV Standard Message Set and libcluon.
# The OpenDLV Standard Message Set contains a set of messages usually used in automotive research project.
set(OPENDLV_STANDARD_MESSAGE_SET assets/opendlv-standard-message-set-v0.9.6.odvd)
# libcluon is a small and portable middleware to easily realize high-performance microservices with C++: https://github.com/chrberger/libcluon
set(CLUON_COMPLETE assets/cluon-complete-v0.0.127.hpp)

################################################################################
# Set the search path for .cmake files.
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
################################################################################
# This project requires C++14 or newer.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# Build a static binary.
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
# Add further warning levels to increase the code quality.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
    -D_XOPEN_SOURCE=700 \
    -D_FORTIFY_SOURCE=2 \
    -O2 \
    -fstack-protector \
    -fomit-frame-pointer \
    -pipe \
    -Weffc++ \
    -Wall -Wextra -Wshadow -Wdeprecated \
    -Wdiv-by-zero -Wfloat-equal -Wfloat-conversion -Wsign-compare -Wpointer-arith \
    -Wuninitialized -Wunreachable-code \
    -Wunused -Wunused-function -Wunused-label -Wunused-parameter -Wunused-but-set-parameter -Wunused-but-set-variable \
    -Wunused-value -Wunused-variable -Wunused-result \
    -Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn")
# Threads are necessary for linking the resulting binaries as the network communication is running inside a thread.
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

################################################################################
# Extract cluon-msc from cluon-complete.hpp.
# cluon-msc is the message compiler that compiles a .odvd message specification into a header-only C++ file.
add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/cluon-msc
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/${CLUON_COMPLETE} ${CMAKE_BINARY_DIR}/cluon-complete.hpp
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_BINARY_DIR}/cluon-complete.hpp ${CMAKE_BINARY_DIR}/cluon-complete.cpp
    COMMAND ${CMAKE_CXX_COMPILER} -o ${CMAKE_BINARY_DIR}/cluon-msc ${CMAKE_BINARY_DIR}/cluon-complete.cpp -std=c++14 -pthread -D HAVE_CLUON_MSC
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${CLUON_COMPLETE})

################################################################################
# Generate opendlv-standard-message-set.hpp from ${OPENDLV_STANDARD_MESSAGE_SET} file.
add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/opendlv-standard-message-set.hpp
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_BINARY_DIR}/cluon-msc --cpp --out=${CMAKE_BINARY_DIR}/opendlv-standard-message-set.hpp ${CMAKE_CURRENT_SOURCE_DIR}/${OPENDLV_STANDARD_MESSAGE_SET}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${OPENDLV_STANDARD_MESSAGE_SET} ${CMAKE_BINARY_DIR}/cluon-msc)
# Add current build directory as include directory as it contains generated files.
include_directories(SYSTEM ${CMAKE_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

################################################################################
# Gather all object code first to avoid double compilation.
set(LIBRARIES Threads::Threads)

if(UNIX)
    if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
        find_package(LibRT REQUIRED)
        set(LIBRARIES ${LIBRARIES} ${LIBRT_LIBRARIES})
        include_directories(SYSTEM ${LIBRT_INCLUDE_DIR})
    endif()
endif()

# This project uses OpenCV for image processing.
find_package(OpenCV REQUIRED core highgui imgproc imgcodecs)
include_directories(SYSTEM ${OpenCV_INCLUDE_DIRS})
set(LIBRARIES ${LIBRARIES} ${OpenCV_LIBS})


################################################################################
# Create executable.
add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/${PROJECT_NAME}.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/ImageCropper.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/ImageFilter.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/ImageTracker.cpp)

add_executable(${PROJECT_NAME}-Tester test/TestImageFilter.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/ImageFilter.cpp test/TestImageCropper.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/ImageCropper.cpp)
add_test(NAME ${PROJECT_NAME}-Tester COMMAND ${PROJECT_NAME}-Tester)

target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
target_link_libraries(${PROJECT_NAME}-Tester ${LIBRARIES})


################################################################################
# Coverage configuration

if(CMAKE_COMPILER_IS_GNUCXX)
include(CodeCoverage)
append_coverage_compiler_flags(-O0)
setup_target_for_coverage_gcovr_summary(
    NAME coverage
    BASE_DIRECTORY "CMAKE_CURRENT_SOURCE_DIR}/src"
    DEPENDENCIES cobertura
    EXCLUDE "build/*"
    EXCLUDE "assets/*"
    EXCLUDE "src/${PROJECT_NAME}.cpp"
    EXCLUDE "test/*")
setup_target_for_coverage_gcovr_xml(
    NAME cobertura
    BASE_DIRECTORY "CMAKE_CURRENT_SOURCE_DIR}/src"
    DEPENDENCIES coverage-reports
    EXCLUDE "build/*"
    EXCLUDE "assets/*"
    EXCLUDE "src/${PROJECT_NAME}.cpp"
    EXCLUDE "test/*")
setup_target_for_coverage_gcovr_html(
    NAME coverage-reports
    EXECUTABLE ${PROJECT_NAME}-Tester
    BASE_DIRECTORY "CMAKE_CURRENT_SOURCE_DIR}/src"
    DEPENDENCIES coverage-reports
    EXCLUDE "build/*"
    EXCLUDE "assets/*"
    EXCLUDE "src/${PROJECT_NAME}.cpp"
    EXCLUDE "test/*")
endif()




# Add dependency to OpenDLV Standard Message Set.
add_custom_target(generate_opendlv_standard_message_set_hpp DEPENDS ${CMAKE_BINARY_DIR}/opendlv-standard-message-set.hpp)
add_dependencies(${PROJECT_NAME} generate_opendlv_standard_message_set_hpp)


################################################################################
# Install executable.
install(TARGETS ${PROJECT_NAME} DESTINATION bin COMPONENT ${PROJECT_NAME})
