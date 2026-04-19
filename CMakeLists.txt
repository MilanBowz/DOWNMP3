cmake_minimum_required(VERSION 3.16)
project(YouTubeDownloader)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Detect platform
if(WIN32)
    set(PLATFORM_WINDOWS TRUE)
    message(STATUS "Building for Windows")
elseif(ANDROID)
    set(PLATFORM_ANDROID TRUE)
    message(STATUS "Building for Android")
elseif(UNIX AND NOT APPLE)
    set(PLATFORM_LINUX TRUE)
    message(STATUS "Building for Linux")
else()
    message(FATAL_ERROR "Unsupported platform!")
endif()

# Platform-specific configurations
if(PLATFORM_LINUX)
    # Linux configuration (unchanged from original)
    find_package(PkgConfig REQUIRED)
    
    pkg_check_modules(WEBKIT webkit2gtk-4.1)
    if(NOT WEBKIT_FOUND)
        pkg_check_modules(WEBKIT webkit2gtk-4.0)
    endif()
    if(NOT WEBKIT_FOUND)
        pkg_check_modules(WEBKIT webkitgtk-6.0)
    endif()
    
    if(NOT WEBKIT_FOUND)
        message(FATAL_ERROR "WebKitGTK not found. Install: sudo apt install libwebkit2gtk-4.0-dev")
    endif()
    
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
    )
    
elseif(PLATFORM_WINDOWS)
    # Windows configuration using WebView2
    add_executable(YouTubeDownloader main.cpp)
    
    target_compile_definitions(YouTubeDownloader PRIVATE WEBVIEW_WIN32)
    target_include_directories(YouTubeDownloader PRIVATE ${CMAKE_SOURCE_DIR})
    
    # Link Windows libraries
    target_link_libraries(YouTubeDownloader
        user32
        ole32
        shell32
        advapi32
        uuid
    )
    
    set_target_properties(YouTubeDownloader PROPERTIES
        OUTPUT_NAME "downloader"
        SUFFIX ".exe"
    )
    
    message(STATUS "For Windows, make sure WebView2Loader.dll is in the executable directory")
    message(STATUS "Download from: https://www.nuget.org/packages/Microsoft.Web.WebView2/")
    
elseif(PLATFORM_ANDROID)
    # Android configuration - create shared library
    add_library(ytdownloader SHARED main.cpp)
    
    target_compile_definitions(ytdownloader PRIVATE WEBVIEW_ANDROID)
    target_include_directories(ytdownloader PRIVATE ${CMAKE_SOURCE_DIR})
    
    # Android specific libraries
    find_library(log-lib log)
    find_library(android-lib android)
    find_library(GLESv2-lib GLESv2)
    find_library(EGL-lib EGL)
    
    target_link_libraries(ytdownloader
        ${log-lib}
        ${android-lib}
        ${GLESv2-lib}
        ${EGL-lib}
    )
    
    # Set Android specific properties
    set_target_properties(ytdownloader PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}
        OUTPUT_NAME "ytdownloader"
    )
    
    message(STATUS "Building for Android ${ANDROID_ABI}")
    message(STATUS "Output will be in: ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libytdownloader.so")
endif()