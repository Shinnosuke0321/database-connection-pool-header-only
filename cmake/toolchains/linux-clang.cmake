set(CMAKE_C_COMPILER clang-19)
set(CMAKE_CXX_COMPILER clang++-19)

# Use libstdc++ on Linux
set(CMAKE_CXX_FLAGS_INIT "-stdlib=libstdc++")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-stdlib=libstdc++")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-stdlib=libstdc++")