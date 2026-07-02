cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME STM32H7)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Bsp/Uart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Third/Lora.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Third/Edge_ai_app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Third/MQTT.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Third/init.c
    # Paho MQTT 库源文件
    ${CMAKE_CURRENT_SOURCE_DIR}/Paho_MQTT/Src/mqtt_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Paho_MQTT/Src/MQTTClient.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Paho_MQTT/Src/MQTTPacket.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Paho_MQTT/Src/MQTTConnectClient.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Paho_MQTT/Src/MQTTConnectServer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Paho_MQTT/Src/MQTTSerializePublish.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Paho_MQTT/Src/MQTTDeserializePublish.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Paho_MQTT/Src/MQTTSubscribeClient.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Paho_MQTT/Src/MQTTSubscribeServer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Paho_MQTT/Src/MQTTUnsubscribeClient.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Paho_MQTT/Src/MQTTUnsubscribeServer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Paho_MQTT/Src/MQTTFormat.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Bsp
    ${CMAKE_CURRENT_SOURCE_DIR}/Third
    ${CMAKE_CURRENT_SOURCE_DIR}/Paho_MQTT/Inc
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
