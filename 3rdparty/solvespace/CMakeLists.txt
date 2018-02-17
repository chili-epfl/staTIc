# cmake configuration

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR
            "In-tree builds are not supported; please perform an out-of-tree build:\n"
            "    rm -rf CMakeCache.txt CMakeFiles/\n"
            "    mkdir build && cd build && cmake ..")
endif()
option(STATIC_LIB "Build Static lib" OFF)

cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
cmake_policy(VERSION 3.1.0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# for /MT on MSVC
set(CMAKE_USER_MAKE_RULES_OVERRIDE
   "${CMAKE_SOURCE_DIR}/cmake/c_flag_overrides.cmake")
set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX
   "${CMAKE_SOURCE_DIR}/cmake/cxx_flag_overrides.cmake")

# project

# NOTE TO PACKAGERS: The embedded git commit hash is critical for rapid bug triage when the builds
# can come from a variety of sources. If you are mirroring the sources or otherwise build when
# the .git directory is not present, please comment the following line:

# and instead uncomment the following, adding the complete git hash of the checkout you are using:
# set(GIT_COMMIT_HASH 0000000000000000000000000000000000000000)


if(NOT CMAKE_C_COMPILER_ID STREQUAL CMAKE_CXX_COMPILER_ID)
    message(FATAL_ERROR "C and C++ compilers should be supplied by the same vendor")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0)
        # GCC 4.8/4.9 ship with broken but present <regex>. meh.
        message(FATAL_ERROR "GCC 5.0+ is required")
    endif()
endif()

# common compiler flags

if(MINGW)
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -static-libgcc")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")

    if(TRIPLE STREQUAL "i686-w64-mingw32")
        set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -msse2")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2")
    endif()
endif()

if(APPLE OR CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    set(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed ${CMAKE_EXE_LINKER_FLAGS}")
endif()


# solvespace-only compiler flags

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(WARNING_FLAGS   "-Wall -Wextra -Wno-unused-parameter")
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(WARNING_FLAGS "${WARNING_FLAGS} -Wfloat-conversion")
    endif()
    # We rely on these -Werror flags.
    set(WARNING_FLAGS   "${WARNING_FLAGS} -Werror=switch")
endif()

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${WARNING_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS}")

# components

add_subdirectory(src)
add_subdirectory(exposed)


