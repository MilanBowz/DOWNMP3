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
    set(WEBVIEW2_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
    # set(WEBVIEW2_LIBRARY_DIR "${CMAKE_SOURCE_DIR}/include/webview2/runtimes/win-x64/native")
    # Windows configuration using WebView2
    add_executable(YouTubeDownloader WIN32 main.cpp)
    
    # Define Windows platform
    target_compile_definitions(YouTubeDownloader PRIVATE 
        WEBVIEW_WIN32
        _CRT_SECURE_NO_WARNINGS
    )
    
    target_include_directories(YouTubeDownloader PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/include)
    
    # Link Windows libraries including WebView2
    target_link_libraries(YouTubeDownloader
        user32
        ole32
        shell32
        advapi32
        uuid
        version
        shlwapi
        # WebView2 is typically loaded dynamically via #include <WebView2.h>
        # The DLL needs to be present at runtime
    )
    
    set_target_properties(YouTubeDownloader PROPERTIES
        OUTPUT_NAME "downloader"
        SUFFIX ".exe"
    )

    if(EXISTS ${CMAKE_SOURCE_DIR}/WebView2Loader.dll)
    else()
        message(WARNING "WebView2Loader.dll not found in project root.")
        message(STATUS "1. Download from: https://www.nuget.org/packages/Microsoft.Web.WebView2/")
        message(STATUS "2. Extract and copy WebView2Loader.dll to: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    endif()

    # Copy yt-dlp.exe to build directory
    if(EXISTS ${CMAKE_SOURCE_DIR}/yt-dlp.exe)
        add_custom_command(TARGET YouTubeDownloader POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_SOURCE_DIR}/yt-dlp.exe
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/yt-dlp.exe
            COMMENT "Copying yt-dlp.exe to output directory"
        )
    else()
        message(WARNING "yt-dlp.exe not found in project root. Download from: https://github.com/yt-dlp/yt-dlp/releases")
    endif()

    message(STATUS "Building for Windows")
    message(STATUS "WebView2 headers: ${CMAKE_SOURCE_DIR}/include")
    
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