# Resolve Homebrew LLVM path
execute_process(
        COMMAND brew --prefix llvm
        OUTPUT_VARIABLE LLVM_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE BREW_RESULT
)

if (NOT BREW_RESULT EQUAL 0)
    message(FATAL_ERROR "Failed to find Homebrew LLVM. Install with: brew install llvm")
endif ()

set(CMAKE_C_COMPILER "${LLVM_PREFIX}/bin/clang")
set(CMAKE_CXX_COMPILER "${LLVM_PREFIX}/bin/clang++")

# Use libc++ on macOS
set(CMAKE_CXX_FLAGS_INIT "-stdlib=libc++")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-stdlib=libc++")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-stdlib=libc++")

# Homebrew LLVM needs explicit sysroot for macOS SDK headers
execute_process(
        COMMAND xcrun --show-sdk-path
        OUTPUT_VARIABLE MACOS_SDK_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

if (MACOS_SDK_PATH)
    set(CMAKE_OSX_SYSROOT "${MACOS_SDK_PATH}")
endif ()