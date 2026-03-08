include(FetchContent)
find_package(GTest QUIET)
if (NOT GTest_FOUND)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.17.0
    )
    FetchContent_MakeAvailable(googletest)
endif ()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Sanitizer option
set(SANITIZER_TYPE "address" CACHE STRING "Sanitizer type (address, thread, undefined, none)")

add_executable(DbConnectionPool_tests db_pool_test.cpp)
add_executable(IntrusivePtr_tests intrusive_ptr_test.cpp)

target_link_libraries(DbConnectionPool_tests PRIVATE
        DbConnectionPool::DbConnectionPool
        GTest::gtest_main
)
target_link_libraries(IntrusivePtr_tests PRIVATE
        DbConnectionPool::DbConnectionPool
        GTest::gtest_main
)

# Apply sanitizer only for supported compilers (Homebrew GCC on macOS does not ship ASan)
if (NOT SANITIZER_TYPE STREQUAL "none")
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU" AND NOT APPLE)

        foreach(target
                DbConnectionPool_tests
                IntrusivePtr_tests
        )
            target_compile_options(${target} PRIVATE
                    -fsanitize=${SANITIZER_TYPE}
                    -fno-omit-frame-pointer
            )

            target_link_options(${target} PRIVATE
                    -fsanitize=${SANITIZER_TYPE}
            )
        endforeach()
    endif()
endif()

include(GoogleTest)
gtest_discover_tests(DbConnectionPool_tests)
gtest_discover_tests(IntrusivePtr_tests)