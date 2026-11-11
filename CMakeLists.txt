cmake_minimum_required(VERSION 3.22)
project(ChatKemer LANGUAGES CXX)

if(NOT ANDROID)
  message(FATAL_ERROR "Build Chat Kemer with the Android NDK")
endif()
if(NOT CMAKE_ANDROID_ARCH_ABI STREQUAL "arm64-v8a")
  message(FATAL_ERROR "Chat Kemer supports arm64-v8a only")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(FetchContent)
FetchContent_Declare(
  preloader_android
  GIT_REPOSITORY https://github.com/LiteLDev/preloader-android.git
  GIT_TAG 0.2.3
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(preloader_android)

add_library(ChatKemer SHARED src/main.cpp)
set_target_properties(ChatKemer PROPERTIES OUTPUT_NAME "ChatKemer")
target_link_libraries(ChatKemer PRIVATE preloader)
target_link_options(ChatKemer PRIVATE "-Wl,-z,max-page-size=16384")
