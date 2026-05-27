cmake_minimum_required(VERSION 3.28)
project(DbConnectionPool VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_OSX_DEPLOYMENT_TARGET "15.0" CACHE STRING "Minimum macOS deployment target")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Clang-specific standard library configuration
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        # Linux Clang: use libstdc++ (GCC's standard library)
        add_compile_options("-stdlib=libstdc++")
        add_link_options("-stdlib=libstdc++")
    elseif (APPLE)
        # macOS Homebrew Clang: use libc++ and set sysroot
        add_compile_options("-stdlib=libc++")
        add_link_options("-stdlib=libc++")

        # Homebrew LLVM doesn't find macOS SDK headers without explicit sysroot
        if (NOT CMAKE_OSX_SYSROOT)
            execute_process(
                    COMMAND xcrun --show-sdk-path
                    OUTPUT_VARIABLE DETECTED_OSX_SYSROOT
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    RESULT_VARIABLE XCRUN_RESULT
            )
            if (XCRUN_RESULT EQUAL 0 AND DETECTED_OSX_SYSROOT)
                set(CMAKE_OSX_SYSROOT "${DETECTED_OSX_SYSROOT}" CACHE PATH "macOS SDK path")
                message(STATUS "Detected macOS SDK sysroot: ${CMAKE_OSX_SYSROOT}")
            endif ()
        endif ()
    endif ()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    # AppleClang uses libc++ by default, no extra configuration needed
endif ()

add_library(DbConnectionPool INTERFACE)

target_include_directories(DbConnectionPool INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

add_library(DbConnectionPool::DbConnectionPool ALIAS DbConnectionPool)

target_compile_features(DbConnectionPool INTERFACE cxx_std_23)

include(CTest)
if (PROJECT_IS_TOP_LEVEL AND BUILD_TESTING)
    add_subdirectory(test)
endif ()

include(GNUInstallDirs)

install(TARGETS DbConnectionPool
        EXPORT DbConnectionPoolTargets
)

install(EXPORT DbConnectionPoolTargets
        NAMESPACE DbConnectionPool::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DbConnectionPool
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)