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

include(pico_sdk_import.cmake)

project(${projectname} C CXX ASM)
pico_sdk_init()

if (NOT HW_CONFIG)
    set(HW_CONFIG 1 CACHE STRING "Select the hardware configuration for your board")       
endif()
if ( HW_CONFIG EQUAL 1 )
	# This default Config is for Pimoroni Pico DV Demo Base
	set(DVICONFIG "dviConfig_PimoroniDemoDVSock" CACHE STRING
	  "Select a default pin configuration from common_dvi_pin_configs.h")
    set(LED_GPIO_PIN "25" CACHE STRING "Select the GPIO pin for LED")
	set(SD_CS "22" CACHE STRING "Specify the Chip Select GPIO pin for the SD card")
	set(SD_SCK "5" CACHE STRING "Specify de Clock GPIO pin for the SD card")
	set(SD_MOSI "18" CACHE STRING "Select the Master Out Slave In GPIO pin for the SD card")
	set(SD_MISO "19" CACHE STRING "Select the Master In Slave Out GPIO pin for the SD card")
	set(NES_CLK "14" CACHE STRING "Select the Clock GPIO pin for NES controller")
	set(NES_DATA "15" CACHE STRING "Select the Data GPIO pin for NES controller")
	set(NES_LAT "16" CACHE STRING "Select the Latch GPIO pin for NES controller")
    set(NES_PIO "pio0" CACHE STRING "Select the PIO for NES controller")
    set(NES_CLK_1 "1" CACHE STRING "Select the Clock GPIO pin for second NES controller")
	set(NES_DATA_1 "21" CACHE STRING "Select the Data GPIO pin for second NES controller")
	set(NES_LAT_1 "20" CACHE STRING "Select the Latch GPIO pin for second NES controller")
    set(NES_PIO_1 "pio1" CACHE STRING "Select the PIO for second NES controller")
	set(WII_SDA "-1" CACHE STRING "Select the SDA GPIO pin for Wii Classic controller")
	set(WII_SCL "-1" CACHE STRING "Select the SCL GPIO pin for Wii Classic controller")
elseif ( HW_CONFIG EQUAL 2 )
	# --------------------------------------------------------------------
	# Alternate config for use with different SDcard reader and HDMI board
    #  Use also for Printed Circuit Board (PCB) version.
	# --------------------------------------------------------------------
	# Adafruit DVI Breakout For HDMI Source Devices https://www.adafruit.com/product/4984
	set(DVICONFIG "dviConfig_PicoDVISock" CACHE STRING
	  "Select a default pin configuration from common_dvi_pin_configs.h")
    set(LED_GPIO_PIN "25" CACHE STRING "Select the GPIO pin for LED")
	# Adafruit Micro-SD breakout board+ https://www.adafruit.com/product/254 
	set(SD_CS "5" CACHE STRING "Specify the Chip Select GPIO pin for the SD card")
	set(SD_SCK "2" CACHE STRING "Specify de Clock GPIO pin for the SD card")
	set(SD_MOSI "3" CACHE STRING "Select the Master Out Slave In GPIO pin for the SD card")
	set(SD_MISO "4" CACHE STRING "Select the Master In Slave Out GPIO pin for the SD card")
	set(NES_CLK "6" CACHE STRING "Select the Clock GPIO pin for NES controller")
	set(NES_DATA "7" CACHE STRING "Select the Data GPIO pin for NES controller")
	set(NES_LAT "8" CACHE STRING "Select the Latch GPIO pin for NES controller")
    set(NES_PIO "pio0" CACHE STRING "Select the PIO for NES controller")
    set(NES_CLK_1 "9" CACHE STRING "Select the Clock GPIO pin for second NES controller")
	set(NES_DATA_1 "10" CACHE STRING "Select the Data GPIO pin for second NES controller")
	set(NES_LAT_1 "11" CACHE STRING "Select the Latch GPIO pin for second NES controller")
    set(NES_PIO_1 "pio1" CACHE STRING "Select the PIO for second NES controller")
	set(WII_SDA "-1" CACHE STRING "Select the SDA GPIO pin for Wii Classic controller")
	set(WII_SCL "-1" CACHE STRING "Select the SCL GPIO pin for Wii Classic controller")
elseif ( HW_CONFIG EQUAL 3 )
	# --------------------------------------------------------------------
	# Alternate config for use with Adafruit Feather RP2040 DVI + SD Wing
	# --------------------------------------------------------------------
	set(DVICONFIG "dviConfig_AdafruitFeatherDVI" CACHE STRING
	  "Select a default pin configuration from common_dvi_pin_configs.h")
    set(LED_GPIO_PIN "13" CACHE STRING "Select the GPIO pin for LED")
	set(SD_CS "10" CACHE STRING "Specify the Chip Select GPIO pin for the SD card")
	set(SD_SCK "14" CACHE STRING "Specify de Clock GPIO pin for the SD card")
	set(SD_MOSI "15" CACHE STRING "Select the Master Out Slave In GPIO pin for the SD card")
	set(SD_MISO "8" CACHE STRING "Select the Master In Slave Out GPIO pin for the SD card")
	set(NES_CLK "5" CACHE STRING "Select the Clock GPIO pin for NES controller")
	set(NES_DATA "6" CACHE STRING "Select the Data GPIO pin for NES controller")
	set(NES_LAT "9" CACHE STRING "Select the Latch GPIO pin for NES controller")
    set(NES_PIO "pio0" CACHE STRING "Select the PIO for NES controller")
    set(NES_CLK_1 "26" CACHE STRING "Select the Clock GPIO pin for second NES controller")
	set(NES_DATA_1 "28" CACHE STRING "Select the Data GPIO pin for second NES controller")
	set(NES_LAT_1 "27" CACHE STRING "Select the Latch GPIO pin for second NES controller")
    set(NES_PIO_1 "pio1" CACHE STRING "Select the PIO for second NES controller")
	set(WII_SDA "2" CACHE STRING "Select the SDA GPIO pin for Wii Classic controller")
	set(WII_SCL "3" CACHE STRING "Select the SCL GPIO pin for Wii Classic controller")
elseif ( HW_CONFIG EQUAL 4 )
    # --------------------------------------------------------------------
	# Alternate config for use with Waveshare RP2040-PiZero
	# --------------------------------------------------------------------
	set(DVICONFIG "dviConfig_WaveShareRp2040" CACHE STRING
    "Select a default pin configuration from common_dvi_pin_configs.h")
    set(LED_GPIO_PIN "-1" CACHE STRING "Select the GPIO pin for LED")
    set(SD_CS "21" CACHE STRING "Specify the Chip Select GPIO pin for the SD card")
    set(SD_SCK "18" CACHE STRING "Specify de Clock GPIO pin for the SD card")
    set(SD_MOSI "19" CACHE STRING "Select the Master Out Slave In GPIO pin for the SD card")
    set(SD_MISO "20" CACHE STRING "Select the Master In Slave Out GPIO pin for the SD card")
    set(NES_CLK "5" CACHE STRING "Select the Clock GPIO pin for NES controller")
    set(NES_DATA "6" CACHE STRING "Select the Data GPIO pin for NES controller")
    set(NES_LAT "9" CACHE STRING "Select the Latch GPIO pin for NES controller")
    set(NES_PIO "pio0" CACHE STRING "Select the PIO for NES controller")
    set(NES_CLK_1 "10" CACHE STRING "Select the Clock GPIO pin for second NES controller")
	set(NES_DATA_1 "12" CACHE STRING "Select the Data GPIO pin for second NES controller")
	set(NES_LAT_1 "11" CACHE STRING "Select the Latch GPIO pin for second NES controller")
    set(NES_PIO_1 "pio1" CACHE STRING "Select the PIO for second NES controller")
    set(WII_SDA "2" CACHE STRING "Select the SDA GPIO pin for Wii Classic controller")
    set(WII_SCL "3" CACHE STRING "Select the SCL GPIO pin for Wii Classic controller")
endif ( )

# --------------------------------------------------------------------
message("HDMI board type       : ${DVICONFIG}")
message("SD card CS            : ${SD_CS}")
message("SD card SCK           : ${SD_SCK}")
message("SD card MOSI          : ${SD_MOSI}")
message("SD card MISO          : ${SD_MISO}")
message("NES controller 0 CLK  : ${NES_CLK}")
message("NES controller 0 DATA : ${NES_DATA}")
message("NES controller 0 LAT  : ${NES_LAT}")
message("NES controller 0 PIO  : ${NES_PIO}")
message("NES controller 1 CLK  : ${NES_CLK_1}")
message("NES controller 1 DATA : ${NES_DATA_1}")
message("NES controller 1 LAT  : ${NES_LAT_1}")
message("NES controller 1 PIO  : ${NES_PIO_1}")
message("Wii controller SDA  : ${WII_SDA}")
message("Wii controller SCL  : ${WII_SCL}")
message("LED pin             : ${LED_GPIO_PIN}")

add_executable(${projectname}
    main.cpp
    hid_app.cpp
    gamepad.cpp
    nespad.cpp
    wiipad.cpp
    menu.cpp 
    RomLister.cpp
    FrensHelpers.cpp
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

# tinyusb
set(FAMILY rp2040)
set(BOARD pico_sdk)
set(TINYUSB_DEBUG_LEVE 0)
include(${PICO_TINYUSB_PATH}/hw/bsp/${FAMILY}/family.cmake)
if (PICO_SDK_VERSION_MAJOR LESS 2)
    family_configure_target(${projectname})
else()
    family_configure_target(${projectname} "" )
endif()
# add_compile_definitions(CFG_TUSB_DEBUG=0)
set(SMS_FLAGS -DMB_SMS -DLSB_FIRST=0
        -DSMS_FILE_ADDR=0x10080000 -DNDEBUG
)
target_compile_definitions(${projectname} PUBLIC 
    ${SMS_FLAGS}
    DVICONFIG=${DVICONFIG}
    SDCARD_PIN_SPI0_CS=${SD_CS}
    SDCARD_PIN_SPI0_SCK=${SD_SCK}
    SDCARD_PIN_SPI0_MOSI=${SD_MOSI}
    SDCARD_PIN_SPI0_MISO=${SD_MISO}
    SDCARD_PIO=pio1
    # SDCARD_PIO_SM=0       # Let the driver decide what state machine to use.
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
    LED_GPIO_PIN=${LED_GPIO_PIN})
    
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
    smsplus
    .
)
add_subdirectory(smsplus)
add_subdirectory(drivers/fatfs)
add_subdirectory(drivers/sdcard)
add_subdirectory(pico_lib/dvi)
add_subdirectory(pico_lib/util)
pico_add_extra_outputs(${projectname})



