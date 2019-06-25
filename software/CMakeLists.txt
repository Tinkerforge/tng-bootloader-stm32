CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(PROJECT_NAME tng-bootloader)
SET(CHIP_FAMILY STM32F0)
SET(CHIP STM32F072xB)
SET(MCU cortex-m0\ -mthumb)
PROJECT(${PROJECT_NAME})

# Optimization level, can be [0, 1, 2, 3, s].
# 0 = turn off optimization. s = optimize for size.
#SET(OPTIMIZATION_LEVEL 0)
SET(DEBUG "-g -ggdb")

SET(OPTIMIZATION_LEVEL 2)
#SET(DEBUG "")

SET(CMAKE_BUILD_TYPE None)
ENABLE_LANGUAGE(C ASM)

INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/"
	"${PROJECT_SOURCE_DIR}/src/bricklib2/stm32cubef0/Drivers/STM32F0xx_HAL_Driver/Inc/"
	"${PROJECT_SOURCE_DIR}/src/bricklib2/stm32cubef0/Drivers/CMSIS/Device/ST/STM32F0xx/Include/"
	"${PROJECT_SOURCE_DIR}/src/bricklib2/stm32cubef0/Drivers/CMSIS/Include/"
)

# find source files
SET(SOURCES
	"${PROJECT_SOURCE_DIR}/src/main.c"

	"${PROJECT_SOURCE_DIR}/src/bricklib2/hal/startup/startup_stm32f0.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib2/hal/startup/startup_stm32f072xb.S"

	"${PROJECT_SOURCE_DIR}/src/bricklib2/stm32cubef0/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_gpio.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib2/stm32cubef0/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_rcc.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib2/stm32cubef0/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_rcc_ex.c"
	"${PROJECT_SOURCE_DIR}/src/bricklib2/stm32cubef0/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pcd"
	"${PROJECT_SOURCE_DIR}/src/bricklib2/stm32cubef0/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pcd_ex"
)

MESSAGE(STATUS "\nFound following source files:\n ${SOURCES}\n")

# define executable
include(${CMAKE_CURRENT_SOURCE_DIR}/src/bricklib2/cmake/configs/config_define_executable.txt)

# define standard compile and linker flags
SET(LINKER_SCRIPT_NAME stm32f0_tng_bootloader.ld)
include(${CMAKE_CURRENT_SOURCE_DIR}/src/bricklib2/cmake/configs/config_tng_add_standard_flags.txt)

# add custom build commands
include(${CMAKE_CURRENT_SOURCE_DIR}/src/bricklib2/cmake/configs/config_tng_add_standard_custom_commands.txt)

# add preprocessor defines
include(${CMAKE_CURRENT_SOURCE_DIR}/src/bricklib2/cmake/configs/config_xmc1_add_preprocessor_defines.txt)
