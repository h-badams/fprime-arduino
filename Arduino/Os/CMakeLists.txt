####
# ArduinoOs:
#
# The Arudino Os support layer uses the F prime baremetal virtualization technology and a custom
# rebuild of the Arduino framework in order to ensure that all the code can compile into a sensible
# binary. This file sets up the Os layer to support all of that.
####
restrict_platforms(ArduinoFw)
add_custom_target("${FPRIME_CURRENT_MODULE}")

arduino_lib_installed("SD" LIB_SD_INSTALLED)

# Set up Arduino implementations
register_os_implementation("Console" Arduino)
register_os_implementation("RawTime" Arduino)
if (LIB_SD_INSTALLED)
    # ESP32 core only: target_use_arduino_libraries does not reliably propagate
    # transitive headers (SD.h -> FS.h etc.) to cmake targets under the ESP32
    # toolchain. Glob the bundled ESP32 core libraries and user-installed
    # libraries so the entire include chain is visible without chasing individual
    # headers. Skipped on other cores (e.g. Feather M4), where the platform
    # provides the SD/Wire include paths directly. Gated by the marker set in
    # cmake/toolchain/esp32s3.cmake.
    if (FPRIME_ARDUINO_CORE_ESP32)
        file(GLOB _ARDUINO_LIB_INCLUDE_DIRS
            "$ENV{HOME}/.arduino15/packages/esp32/hardware/esp32/*/libraries/*/src"
            "$ENV{HOME}/Arduino/libraries/*/src"
        )
        foreach(_DIR ${_ARDUINO_LIB_INCLUDE_DIRS})
            include_directories("${_DIR}")
        endforeach()
    endif()
    target_use_arduino_libraries("SD")
    register_os_implementation("File;FileSystem;Directory" Arduino_SD)
endif()
