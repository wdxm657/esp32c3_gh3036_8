cmake_minimum_required(VERSION 3.15)


# --- gh_hal sources ---
file(GLOB_RECURSE GH_HAL_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/gh_hal/*.c
)

# --- gh_app sources (optional) ---
set(GH_APP_SRCS)
if(CONFIG_GH_USE_SDK_APP)
    file(GLOB_RECURSE GH_APP_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/gh_app/src/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/gh_app/src/**/*.c
    )
    file(GLOB_RECURSE GH_ALGO_ADT_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/gh_algo/adt/*.c)
    list(APPEND GH_APP_SRCS ${GH_ALGO_ADT_SRCS})
endif()

# --- gh_protocol sources (optional) ---
set(GH_PROTOCOL_SRCS)
if(CONFIG_GH_USE_SDK_APP AND CONFIG_GH_PROTOCOL_EN)
    file(GLOB_RECURSE GH_PROTOCOL_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/gh_protocol/*.c
    )
endif()

# Include dirs exposed to consumers
set(GH_SDK_INC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/gh_auto_config
    ${CMAKE_CURRENT_SOURCE_DIR}/gh_hal
    ${CMAKE_CURRENT_SOURCE_DIR}/gh_hal/hw_interface/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/gh_hal/hw_service/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/gh_app/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/gh_app/src
    ${CMAKE_CURRENT_SOURCE_DIR}/gh_algo
    ${CMAKE_CURRENT_SOURCE_DIR}/gh_algo/common
    ${CMAKE_CURRENT_SOURCE_DIR}/gh_algo/adt
    ${CMAKE_CURRENT_SOURCE_DIR}/gh_algo/hrv
    ${CMAKE_CURRENT_SOURCE_DIR}/gh_algo/nadt
    ${CMAKE_CURRENT_SOURCE_DIR}/gh_protocol
)

# --- gh_algo sources (optional + selectable level) ---
set(GH_ALGO_SRCS)
if(CONFIG_GH_USE_SDK_APP AND CONFIG_GH_ALGO_EN)
    if (NOT CONFIG_GH_USE_HAL_COMMON_LIB)
        message(FATAL_ERROR "CONFIG_GH_USE_HAL_COMMON_LIB is not defined. Use Algo with HAL Common Lib.")
    endif()
    
    # Use Kconfig-provided algorithm version choices. These map to
    # CONFIG_GOODIX_HR_ALGO_VERISON and CONFIG_GOODIX_SPO2_ALGO_VERISON.
    # If they are not defined at configure-time, fall back to EXCLUSIVE(4).
    if(DEFINED CONFIG_GOODIX_HR_ALGO_VERISON)
        set(HR_VER ${CONFIG_GOODIX_HR_ALGO_VERISON})
    else()
        set(HR_VER 4)
    endif()

    if(DEFINED CONFIG_GOODIX_SPO2_ALGO_VERISON)
        set(SPO2_VER ${CONFIG_GOODIX_SPO2_ALGO_VERISON})
    else()
        set(SPO2_VER 4)
    endif()

    # Determine which common subdir to use: if either HR or SPO2 is BASIC(0)
    # or MEDIUM(2) then use the basic_medium common code; otherwise use
    # premium_exclusive.
    if(HR_VER EQUAL 0 OR HR_VER EQUAL 2 OR SPO2_VER EQUAL 0 OR SPO2_VER EQUAL 2)
        set(GH_COMMON_SUBDIR "basic_medium")
    else()
        set(GH_COMMON_SUBDIR "premium_exclusive")
    endif()

    # Map HR version to HR subdir
    if(HR_VER EQUAL 0)
        set(GH_HR_SUBDIR "basic")
    elseif(HR_VER EQUAL 2)
        set(GH_HR_SUBDIR "medium")
    elseif(HR_VER EQUAL 3)
        set(GH_HR_SUBDIR "premium")
    else()
        set(GH_HR_SUBDIR "exclusive")
    endif()

    # Map SPO2 version to SPO2 subdir (0/2 -> basic, else premium)
    if(SPO2_VER EQUAL 0 OR SPO2_VER EQUAL 2)
        set(GH_SPO2_SUBDIR "basic")
    else()
        set(GH_SPO2_SUBDIR "premium")
    endif()

    file(GLOB_RECURSE GH_ALGO_COMMON_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/gh_algo/common/${GH_COMMON_SUBDIR}/*.c)
    file(GLOB_RECURSE GH_ALGO_HR_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/gh_algo/hr/${GH_HR_SUBDIR}/*.c)
    file(GLOB_RECURSE GH_ALGO_HRV_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/gh_algo/hrv/*.c)
    file(GLOB_RECURSE GH_ALGO_NADT_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/gh_algo/nadt/*.c)
    file(GLOB_RECURSE GH_ALGO_SPO2_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/gh_algo/spo2/${GH_SPO2_SUBDIR}/*.c)

    # Add specific algorithm include subdirs depending on level
    if(CONFIG_GH_USE_SDK_APP AND CONFIG_GH_ALGO_EN)
    list(APPEND GH_SDK_INC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/gh_algo/common/${GH_COMMON_SUBDIR})
    list(APPEND GH_SDK_INC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/gh_algo/hr/${GH_HR_SUBDIR})
    list(APPEND GH_SDK_INC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/gh_algo/spo2/${GH_SPO2_SUBDIR})
    endif()

    set(GH_HAL_COMMON_DRV_LIB_NAME "${CMAKE_CURRENT_SOURCE_DIR}/gh_algo/hal/${CONFIG_GH_LIB_NAME_PREFIX}gh3036drv.${CONFIG_GH_LIB_NAME_SUFFIX}")
    set(GH_ALGO_COMMON_DSP_LIB_NAME "${CMAKE_CURRENT_SOURCE_DIR}/gh_algo/common/${CONFIG_GH_LIB_NAME_PREFIX}common_dsp.${CONFIG_GH_LIB_NAME_SUFFIX}")
    set(GH_ALGO_COMMON_DL_LIB_NAME "${CMAKE_CURRENT_SOURCE_DIR}/gh_algo/common/${GH_COMMON_SUBDIR}/${CONFIG_GH_LIB_NAME_PREFIX}common_dl.${CONFIG_GH_LIB_NAME_SUFFIX}")
    set(GH_ALGO_HR_LIB_NAME "${CMAKE_CURRENT_SOURCE_DIR}/gh_algo/hr/${GH_HR_SUBDIR}/${CONFIG_GH_LIB_NAME_PREFIX}hr.${CONFIG_GH_LIB_NAME_SUFFIX}")
    set(GH_ALGO_SPO2_LIB_NAME "${CMAKE_CURRENT_SOURCE_DIR}/gh_algo/spo2/${GH_SPO2_SUBDIR}/${CONFIG_GH_LIB_NAME_PREFIX}spo2.${CONFIG_GH_LIB_NAME_SUFFIX}")
    set(GH_ALGO_HRV_LIB_NAME "${CMAKE_CURRENT_SOURCE_DIR}/gh_algo/hrv/${CONFIG_GH_LIB_NAME_PREFIX}hrv.${CONFIG_GH_LIB_NAME_SUFFIX}")
    set(GH_ALGO_NADT_LIB_NAME "${CMAKE_CURRENT_SOURCE_DIR}/gh_algo/nadt/${CONFIG_GH_LIB_NAME_PREFIX}nadt.${CONFIG_GH_LIB_NAME_SUFFIX}")

    list(APPEND GH_ALGO_SRCS ${GH_ALGO_COMMON_SRCS} ${GH_ALGO_HR_SRCS} ${GH_ALGO_HRV_SRCS} ${GH_ALGO_NADT_SRCS} ${GH_ALGO_SPO2_SRCS})

    # Also include any other C files under gh_algo that were not matched above
    # file(GLOB_RECURSE GH_ALGO_OTHER_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/gh_algo/*/*.c)
    # foreach(f ${GH_ALGO_OTHER_SRCS})
    #     list(FIND GH_ALGO_SRCS ${f} _found)
    #     if(_found EQUAL -1)
    #         list(APPEND GH_ALGO_SRCS ${f})
    #     endif()
    # endforeach()

    add_library(gh_algo_params STATIC ${GH_ALGO_SRCS})
    target_include_directories(gh_algo_params PUBLIC
        ${GH_SDK_INC_DIRS}
    )
endif()

# Combine all sources for the single idf component
set(GH_SDK_SRCS)
list(APPEND GH_SDK_SRCS ${GH_HAL_SRCS} ${GH_APP_SRCS} ${GH_PROTOCOL_SRCS})



# Register the component with ESP-IDF

if (NOT CONFIG_GH3036_SDK_ENABLE)
idf_component_register()
else()
add_compile_definitions(CONFIG_GH_AUTO_CONFIG_H="${CONFIG_GH_AUTO_CONFIG_H}")
add_compile_definitions(CONFIG_AUTO_CONFIG_H="${CONFIG_AUTO_CONFIG_H}")
idf_component_register(SRCS ${GH_SDK_SRCS}
                       INCLUDE_DIRS ${GH_SDK_INC_DIRS}
                       PRIV_REQUIRES esp_driver_spi esp_driver_gpio
                       )
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-incompatible-pointer-types)
target_compile_options(${COMPONENT_LIB} PUBLIC -DCONFIG_GH_AUTO_CONFIG_H="${CONFIG_GH_AUTO_CONFIG_H}" -DCONFIG_AUTO_CONFIG_H="${CONFIG_AUTO_CONFIG_H}")

if (CONFIG_GH_USE_SDK_APP AND CONFIG_GH_ALGO_EN)
    target_link_libraries(${COMPONENT_LIB} PRIVATE 
        -Wl,--start-group
        gh_algo_params
        ${GH_HAL_COMMON_DRV_LIB_NAME}
        ${GH_ALGO_COMMON_DSP_LIB_NAME}
        ${GH_ALGO_COMMON_DL_LIB_NAME}
        ${GH_ALGO_HR_LIB_NAME}
        ${GH_ALGO_SPO2_LIB_NAME}
        ${GH_ALGO_HRV_LIB_NAME}
        ${GH_ALGO_NADT_LIB_NAME}
        -Wl,--end-group
    )
endif()
message(STATUS "Configured gh3036_sdk as ESP-IDF component. Sources count = ${CMAKE_ARGV0}")
message(STATUS "----------------------------------------")
endif()

# 定义序号计数器，初始值为 1
set(src_index 1)

# 带序号遍历打印每个源文件
foreach(src_file IN LISTS GH_SDK_SRCS)
    message(STATUS "[${src_index}]: ${src_file}")
    # 序号自增 1
    math(EXPR src_index "${src_index} + 1")
endforeach()

message(STATUS "----------------------------------------")
