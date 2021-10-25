cmake_minimum_required(VERSION 3.20)
project(TestMqttCpp)

set(CMAKE_CXX_STANDARD 14)

add_executable(TestMqttCpp main.cpp callback.h callback.cpp action_listener.cpp action_listener.h)
target_link_libraries(TestMqttCpp paho-mqttpp3 paho-mqtt3as modbus)