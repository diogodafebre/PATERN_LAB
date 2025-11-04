cmake_minimum_required(VERSION 3.13.1)

# Path to out-of-tree boards/shield definitions
set(BOARD_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

set(SHIELD f7disco_extension_board)

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
#find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(ButtonManager)

set(PROJECT_SOURCE_PATH "../src")

file(GLOB project_sources ${PROJECT_SOURCE_PATH}/main.cpp)

target_sources(app PRIVATE ${project_sources} ${app_sources})
