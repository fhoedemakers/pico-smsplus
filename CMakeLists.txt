cmake_minimum_required(VERSION 3.13)
set(projectname picosmsPlus)
#set(PICO_DEOPTIMIZED_DEBUG on)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(pico_sdk_import.cmake)

project(${projectname} C CXX ASM)
pico_sdk_init()

add_executable(${projectname}
    shell/main.cpp
)

pico_set_program_name(${projectname} "${projectname}")
pico_set_program_version(${projectname} "0.1")

pico_enable_stdio_uart(${projectname} 1)
pico_enable_stdio_usb(${projectname} 0)

# tinyusb
set(FAMILY rp2040)
set(BOARD pico_sdk)
set(TINYUSB_DEBUG_LEVE 0)
include(${PICO_TINYUSB_PATH}/hw/bsp/${FAMILY}/family.cmake)
family_configure_target(${projectname})
# add_compile_definitions(CFG_TUSB_DEBUG=0)

target_link_libraries(${projectname}
PRIVATE
    pico_stdlib
    pico_multicore
    hardware_dma
    hardware_pio
    hardware_i2c
    hardware_interp
    hardware_timer
    hardware_clocks
    hardware_pwm
    hardware_flash
    smscore
    fatfs
    sdcard
    tinyusb_host
    dvi
    util
)

target_include_directories(${projectname}
PRIVATE
    pico_lib
    core
    shell
    .
)
add_subdirectory(core)
add_subdirectory(drivers/fatfs)
add_subdirectory(drivers/sdcard)
add_subdirectory(pico_lib/dvi)
add_subdirectory(pico_lib/util)
pico_add_extra_outputs(${projectname})



