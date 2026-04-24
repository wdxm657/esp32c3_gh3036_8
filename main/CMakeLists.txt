set(srcs "ppg_main.c")

# Include user's additional source files under main/user/src
# Note: CONFIGURE_DEPENDS is not valid in script/find-package modes used by
# ESP-IDF component handling. Use a plain GLOB_RECURSE so CMake remains
# compatible with the component build flow.
file(GLOB_RECURSE USER_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/user/src/*.c)

idf_component_register(SRCS ${srcs} ${USER_SRCS}
                       PRIV_REQUIRES esp_driver_spi esp_driver_gpio esp_timer freertos gh3036_sdk
                       INCLUDE_DIRS "." "user/inc")
