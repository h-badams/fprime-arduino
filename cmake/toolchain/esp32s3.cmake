####
# esp32s3.cmake:
#
# ESP32-S3 Dev Module support.
# Based on esp32.cmake with S3-specific FQBN.
####
cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 20)
add_compile_options(-std=c++2a -std=gnu++2a) # Force C++20 standard

set(CMAKE_SYSTEM_NAME "Generic")
set(CMAKE_SYSTEM_PROCESSOR "arm")
set(CMAKE_CROSSCOMPILING 1)
set(FPRIME_PLATFORM "ArduinoFw")
set(FPRIME_USE_BAREMETAL_SCHEDULER ON)

# Marker consumed by board-gated code in the shared fprime-arduino tree
# (e.g. Arduino/Os/CMakeLists.txt). Other toolchains (e.g. FeatherM4_FreeRTOS)
# leave this unset so ESP32-only build steps are skipped. The matching
# compile-time gate is ARDUINO_ARCH_ESP32, defined by the ESP32 Arduino core.
set(FPRIME_ARDUINO_CORE_ESP32 ON CACHE BOOL "Building against the ESP32 Arduino core")

# Prevent test program compiling
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

set(ARDUINO_FQBN "esp32:esp32:esp32s3:EraseFlash=all")
add_compile_options(-D_BOARD_ESP32S3 -DNO_ONBOARD_LED -DUSE_BASIC_TIMER)
# Run the base arduino setup which should detect settings!
include("${CMAKE_CURRENT_LIST_DIR}/support/arduino-support.cmake")
