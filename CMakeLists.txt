cmake_minimum_required(VERSION 3.16)
project(YouTubeDownloader)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Find GTK and WebKit
find_package(PkgConfig REQUIRED)

# Try different WebKit versions
pkg_check_modules(WEBKIT webkit2gtk-4.1)
if(NOT WEBKIT_FOUND)
    pkg_check_modules(WEBKIT webkit2gtk-4.0)
endif()
if(NOT WEBKIT_FOUND)
    pkg_check_modules(WEBKIT webkitgtk-6.0)
endif()

if(NOT WEBKIT_FOUND)
    message(FATAL_ERROR "WebKitGTK not found. Install: sudo pacman -S webkit2gtk")
endif()

# Create executable
add_executable(YouTubeDownloader main.cpp)

target_compile_options(YouTubeDownloader PRIVATE ${WEBKIT_CFLAGS})
target_include_directories(YouTubeDownloader PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(YouTubeDownloader
    ${WEBKIT_LDFLAGS}
    pthread
    dl
)

set_target_properties(YouTubeDownloader PROPERTIES
    OUTPUT_NAME "downloader"
    SUFFIX ".exe"
)