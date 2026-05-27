set(CMAKE_C_COMPILER cl)
set(CMAKE_CXX_COMPILER cl)

# MSVC-specific flags if needed
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")