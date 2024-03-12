cmake_minimum_required(VERSION 3.10)

project(DA2324_PRJ1_G184)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES
    src/Node.cpp
    src/WaterNetwork.cpp
    src/App.cpp
    src/main.cpp
)

include_directories(inc)

add_executable(DA2324_PRJ1_G184 ${SOURCE_FILES})
