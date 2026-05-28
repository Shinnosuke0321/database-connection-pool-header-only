set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_OSX_DEPLOYMENT_TARGET "15.0")

if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "21")
    message(WARNING "setting experimental-library flag for apple clang versioned less than 21")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -fexperimental-library")
endif ()