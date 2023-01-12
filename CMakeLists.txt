cmake_policy(SET CMP0048 NEW)
project(Nucleo-F439ZI C CXX ASM)
cmake_minimum_required(VERSION 3.20)

add_library(stm32hal
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c
  Middlewares/Third_Party/FreeRTOS/Source/croutine.c
  Middlewares/Third_Party/FreeRTOS/Source/event_groups.c
  Middlewares/Third_Party/FreeRTOS/Source/list.c
  Middlewares/Third_Party/FreeRTOS/Source/queue.c
  Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c
  Middlewares/Third_Party/FreeRTOS/Source/tasks.c
  Middlewares/Third_Party/FreeRTOS/Source/timers.c
  Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.c
  Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c
  Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_hash.c
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_hash_ex.c)

target_include_directories(stm32hal PUBLIC
  Core/Inc
  Drivers/STM32F4xx_HAL_Driver/Inc
  Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
  Drivers/CMSIS/Device/ST/STM32F4xx/Include
  Drivers/CMSIS/Include
  Middlewares/Third_Party/FreeRTOS/Source/include
  Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2
  Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F)

target_link_libraries(stm32hal PUBLIC
  c
  m
  nosys)

target_compile_definitions(stm32hal PUBLIC
  -DUSE_HAL_DRIVER
  -DSTM32F439xx)

add_library(projbase OBJECT
  startup_stm32f439xx.s
  Core/Src/system_stm32f4xx.c
  Core/Src/stm32f4xx_hal_timebase_tim.c
  Core/Src/freertos.c
  Core/Src/main.c
  Core/Src/entry.cpp
  # the following two have to be linked as object files to override symbols from stm32hal
  Core/Src/stm32f4xx_hal_msp.c
  Core/Src/stm32f4xx_it.c)

target_link_libraries(projbase PUBLIC stm32hal)

get_filename_component(LINKER_SCRIPT STM32F439ZITx_FLASH.ld ABSOLUTE)

function(add_stm32_executable name source)
  set(elf_file ${name}.elf)
  set(bin_file ${name}.bin)
  set(hex_file ${name}.hex)
  set(map_file ${name}.map)
  set(lss_file ${name}.lss)

  add_executable(${elf_file} ${source})

  target_link_libraries(${elf_file} PRIVATE
    projbase
    -Wl,-Map=${map_file},--cref,--no-warn-mismatch
    -Wl,--gc-sections
    -T${LINKER_SCRIPT})

  target_link_libraries(${elf_file} PRIVATE ${additional_linker_flags})

  # show size of resulting firmware image
  add_custom_target(${elf_file}-size DEPENDS ${elf_file} COMMAND ${ARM_SIZE_EXECUTABLE} -B ${elf_file})

  # generate extended listing
  add_custom_target(${lss_file} DEPENDS ${elf_file} COMMAND ${ARM_OBJDUMP_EXECUTABLE} -S ${elf_file} > ${lss_file})

  # create binary and hex files
  add_custom_target(${hex_file} DEPENDS ${elf_file} COMMAND ${ARM_OBJCOPY_EXECUTABLE} -Oihex ${elf_file} ${hex_file})
  add_custom_target(${bin_file} DEPENDS ${elf_file} COMMAND ${ARM_OBJCOPY_EXECUTABLE} -Obinary ${elf_file} ${bin_file})

  # create custom flash target
  add_custom_target(${name}-flash DEPENDS ${bin_file} COMMAND st-flash --reset write ${bin_file} 0x8000000)

  add_custom_target(${name} ALL DEPENDS ${elf_file}-size ${bin_file} ${hex_file} ${lss_file})
endfunction()

add_stm32_executable(stm32crypto Core/Src/crypto.cpp)
add_stm32_executable(stm32calc Core/Src/calculator.cpp)
