cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(projectname pico-smsplus)
include(pico_sdk_import.cmake)
project(${projectname} C CXX ASM)
pico_sdk_init()
pico_set_program_name(${projectname} "pico-smsplus")
pico_set_program_version(${projectname} "0.1")

pico_enable_stdio_uart(${projectname} 1)
pico_enable_stdio_usb(${projectname} 0)

# tinyusb
set(FAMILY rp2040)
set(BOARD pico_sdk)
set(TINYUSB_DEBUG_LEVE 0)
include(${PICO_TINYUSB_PATH}/hw/bsp/${FAMILY}/family.cmake)
family_configure_target(${projectname})
add_subdirectory(core)
add_subdirectory(drivers/fatfs)
add_subdirectory(drivers/sdcard)
# Add your source files here
set(SOURCES
    shell/main.cpp
)


# Add your target executable here
add_executable(${projectname}  ${SOURCES})

target_link_libraries(${projectname} PRIVATE
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
    dvi
    util
    smscore
    tinyusb_host
    sdcard
    fatfs        
)

target_include_directories(${projectname}
    PRIVATE
        pico_lib
        smscore
        include
        .
    )
