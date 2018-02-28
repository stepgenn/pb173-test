#cmake_minimum_required(VERSION 3.6)
#project(pb173_test)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#set(SOURCE_FILES main.cpp)
#add_executable(pb173_test ${SOURCE_FILES})

cmake_minimum_required(VERSION 3.6)
project(pb173_test)
#include_directories(mbedtls)
#link_directories(mbedtls)

#add_executable(MBEDTLS_LIB aes.c ctr_drbg.c)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp func.cpp aes.c aesni.c ctr_drbg.c entropy.c entropy_poll.c platform.c sha512.c timing.c)
add_executable(pb173_test ${SOURCE_FILES})