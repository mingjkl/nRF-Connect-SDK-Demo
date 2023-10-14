# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(include)

target_sources(app PRIVATE src/main.c src/test_1/sum.c)     # add sum.c to the build
include_directories(sum.h src/test_1)                       # add sum.h to the build


file(GLOB app_sources src/test_2/*.c)                       # find all .c files in test_2
target_sources(app PRIVATE ${app_sources})                  # add all .c files in test_2 to the build
include_directories(src/test_2/include)                     # add include directory to the build