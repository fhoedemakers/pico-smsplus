cmake_minimum_required(VERSION 3.13)
set(projectname picosmsPlus)
#set(PICO_DEOPTIMIZED_DEBUG on)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

IF(NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE Debug
       CACHE STRING "Choose the type of build : None Debug Release RelWithDebInfo MinSizeRel."
       FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)
message("* Current build type is : ${CMAKE_BUILD_TYPE}")


if (NOT HW_CONFIG)
    set(HW_CONFIG 2 CACHE STRING "Select the hardware configuration for your board")       
endif()
include("pico_shared/BoardConfigs.cmake")
if (NOT PICO_BOARD )
    set(PICO_BOARD pico CACHE STRING "Board type")
    message("PICO_BOARD not set, using default: ${PICO_BOARD}")
endif()
add_definitions( -DNDEBUG )
include(${PICO_SDK_PATH}/lib/tinyusb/hw/bsp/family_support.cmake)
project(${projectname} C CXX ASM)
# NOTE: pico_sdk_import.cmake is imported by family_initialize_project()
#       family_initialize_project() also calls pico_sdk_init().
#       latest master branch of pico-sdk is required for PIO USB support.
family_initialize_project(${projectname} ${CMAKE_CURRENT_LIST_DIR})

add_executable(${projectname}
    main.cpp
    splash.cpp
)

pico_set_program_name(${projectname} "${projectname}")
pico_set_program_version(${projectname} "0.1")

# disable uart output for Pimoronis Pico DV Demo Base, as gpio 1 is used for NES controller
# so debug messages will not be visible on the serial console.
if ( HW_CONFIG EQUAL 1 )
    message("Disabling UART output for Pimoroni Pico DV Demo Base")
    pico_enable_stdio_uart(${projectname} 0)
else()
    pico_enable_stdio_uart(${projectname} 1)
endif()
pico_enable_stdio_usb(${projectname} 0)

family_configure_target(${projectname} "" )

# add_compile_definitions(CFG_TUSB_DEBUG=0)
set(SMS_FLAGS -DMB_SMS -DLSB_FIRST=0
        -DSMS_FILE_ADDR=0x10080000 -DNDEBUG
)
target_compile_definitions(${projectname} PUBLIC 
    ${SMS_FLAGS}
    DVICONFIG=${DVICONFIG}
    SDCARD_PIN_CS=${SD_CS}
    SDCARD_PIN_SCK=${SD_SCK}
    SDCARD_PIN_MOSI=${SD_MOSI}
    SDCARD_PIN_MISO=${SD_MISO}
    SDCARD_PIO=pio1        # Fallback when SPI initialization fails
    SDCARD_SPI=${SD_SPI}  
    NES_PIN_CLK=${NES_CLK}
    NES_PIN_DATA=${NES_DATA}
    NES_PIN_LAT=${NES_LAT}
    NES_PIO=${NES_PIO}
    NES_PIN_CLK_1=${NES_CLK_1}
    NES_PIN_DATA_1=${NES_DATA_1}
    NES_PIN_LAT_1=${NES_LAT_1}
    NES_PIO_1=${NES_PIO_1}
    WII_PIN_SDA=${WII_SDA}
    WII_PIN_SCL=${WII_SCL}
    LED_GPIO_PIN=${LED_GPIO_PIN}
    HW_CONFIG=${HW_CONFIG}
    WIIPAD_I2C=${WIIPAD_I2C}
    USE_I2S_AUDIO=${USE_I2S_AUDIO}
    PICO_AUDIO_I2S_DATA_PIN=${PICO_AUDIO_I2S_DATA_PIN}
    PICO_AUDIO_I2S_CLOCK_PIN_BASE=${PICO_AUDIO_I2S_CLOCK_PIN_BASE}
    PICO_AUDIO_I2S_PIO=${PICO_AUDIO_I2S_PIO}
    PICO_AUDIO_I2S_DMA_IRQ=1
    PICO_AUDIO_I2S_CLOCK_PINS_SWAPPED=${PICO_AUDIO_I2S_CLOCK_PINS_SWAPPED}
    CFG_TUH_RPI_PIO_USB=${ENABLE_PIO_USB} # Enable PIO USB support
    PIO_USB_USE_PIO=${PIO_USB_USE_PIO} # Use PIO for USB
    PSRAM_CS_PIN=${PSRAM_CS_PIN} # Select the GPIO pin for PSRAM chip select
)
if (ENABLE_PIO_USB EQUAL 1)
    # if PIO_DP_PLUS_PIN = -1, then use the default PIO USB DP pin from the board config.
    if ( NOT  PIO_DP_PLUS_PIN EQUAL -1 )
        message(STATUS "Using PIO USB DP pin: ${PIO_DP_PLUS_PIN}")
        target_compile_definitions(${projectname} PRIVATE
        PICO_DEFAULT_PIO_USB_DP_PIN=${PIO_DP_PLUS_PIN} # Use the specified pin for PIO USB DP
        )
    endif()
    message("Adding target link libraries for PIO USB support.")
    target_link_libraries(${projectname} PRIVATE tinyusb_pico_pio_usb)
endif()    
target_link_libraries(${projectname}
PRIVATE
    smscore
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
    tinyusb_host 
    pico_shared  
    xinput_host 
)
if(PICO_BOARD STREQUAL "pico_w" OR PICO_BOARD STREQUAL "pico2_w")
    # Attention pico2_w in arm-s or risv-s mode causes the led blinking to generate red screen flicker and
    # multiple "[CYW43] do_ioctl(2, 263, 16): timeout" messages to the console.
    # pico_w does not have this issue.
    target_link_libraries(${projectname} PRIVATE pico_cyw43_arch_none)
endif()

target_include_directories(${projectname}
PRIVATE
    pico_lib
    smsplus
    tusb_xinput
    pico_shared
    .
)
add_subdirectory(smsplus)
add_subdirectory(pico_lib/dvi)
add_subdirectory(pico_lib/util)
add_subdirectory(pico_shared)
add_subdirectory(tusb_xinput xinput_host)
target_link_options(${projectname} PRIVATE -Xlinker --print-memory-usage --data-sections)
pico_add_extra_outputs(${projectname})



