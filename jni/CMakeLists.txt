cmake_minimum_required(VERSION 3.20)
project(libdatachannel_jni C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

option(PROJECT_VERSION "The version of the project" "unspecified")

set(NO_WEBSOCKET ON CACHE BOOL "configure libdatachannel build")
set(NO_MEDIA ON CACHE BOOL "configure libdatachannel build")
set(NO_TESTS ON CACHE BOOL "configure libdatachannel build")
set(NO_EXAMPLES ON CACHE BOOL "configure libdatachannel build")
add_subdirectory(libdatachannel)

include_directories(libdatachannel/include generated)
include_directories(jdk)
if(WIN32)
    include_directories(jdk/windows)
else()
    include_directories(jdk/unix)
endif()


add_compile_options(-Werror -Wall -Wextra -Wno-unused-parameter -fPIC -D "PROJECT_VERSION=${PROJECT_VERSION}")
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -flto)
else()
    add_compile_options(-g3 -Og)
endif()

add_link_options(-fvisibility=hidden)
if(ANDROID)
    add_link_options(-z noexecstack -static-libgcc -static-libstdc++)
elseif(NOT WIN32)
    add_link_options(-z noexecstack)
else()
    add_link_options(-static)
endif()

add_library(datachannel-java SHARED
        src/init.c
        src/util.c
        src/native_channel.c
        src/native_peer.c
        src/native_track.c
        src/callback.c
        src/jni-c-to-java.c)
target_link_libraries(datachannel-java PRIVATE datachannel-static)
