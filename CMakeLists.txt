cmake_minimum_required(VERSION 3.20)
project(tg_bot)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_TOOLCHAIN_FILE "C:/Users/79537/Documents/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

find_package(Boost REQUIRED COMPONENTS property_tree)

# Явно укажем пути к tgbot-cpp
find_path(TGBOT_INCLUDE_DIR tgbot/tgbot.h
    PATHS ${CMAKE_TOOLCHAIN_FILE}/../packages/tgbot-cpp_x64-windows/include
    PATH_SUFFIXES .
)

find_library(TGBOT_LIBRARY
    NAMES TgBot
    PATHS ${CMAKE_TOOLCHAIN_FILE}/../packages/tgbot-cpp_x64-windows/lib
)

add_executable(tg_bot main.cpp)

target_include_directories(tg_bot PRIVATE ${TGBOT_INCLUDE_DIR})
target_link_libraries(tg_bot PRIVATE ${TGBOT_LIBRARY} Boost::property_tree)
