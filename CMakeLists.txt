###########################################################
# PlusIrbis project
# Alexey Mironov, 2018-2020
###########################################################

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

option(WITH_SHARED_LIBRARIES   "Build with shared libraries"        OFF)
option(WITH_TESTS              "Build with tests"                   ON)
option(WITH_COVERAGE           "Build with code coverage"           OFF)
option(WITH_APPS               "Build with apps"                    ON)
option(WITH_EXAMPLES           "Build with examples"                ON)
option(WITH_SANITIZERS         "Build with sanitizers"              OFF)
option(WITH_LTO                "Build with link-time optimizations" OFF)
option(WITH_PEDANTIC           "Build with pedantic checks"         OFF)
option(WITHOUT_STRICT_ALIASING "Build without strict-aliasing"      OFF)

# top level project
project(PlusIrbis)

# force C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)

# compiler-dependent definitions
if(MSVC)
    add_compile_options(/UTF-8 /permissive-)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    if (WITH_LTO)
        add_compile_options(/GL /LTCG)
    endif()
else()
    # g++ or clang++
    if(${CMAKE_VERSION} VERSION_LESS "3.1.0")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11") # for old CMake
    endif()
    if (WITH_COVERAGE)
      set (CMAKE_CXX_FLAGS    "${CMAKE_CXX_FLAGS} --coverage")
      set (CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} --coverage")
    endif()
    if (WITH_SANITIZERS)
        set (CMAKE_CXX_FLAGS    "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -fsanitize=address")
        set (CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fno-omit-frame-pointer -fsanitize=address")
    endif()
    if (WITH_PEDANTIC)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wshadow -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op -Wshift-overflow=2 -Wduplicated-cond -Wcast-qual -Wcast-align -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_FORTIFY_SOURCE=2")
    endif()
    if (WITHOUT_STRICT_ALIASING)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")
    if (MINGW)
        add_definitions(-DIRBIS_MINGW)
    endif()
endif()

# headers of irbis and third-party libraries
include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/third)

# where artifacts will be gathered
set(ARTIFACTS ${PROJECT_SOURCE_DIR}/artifacts)

# installation settings
install(FILES ${PROJECT_SOURCE_DIR}/include/irbis.h DESTINATION ${ARTIFACTS})
install(FILES ${PROJECT_SOURCE_DIR}/include/rfid.h DESTINATION  ${ARTIFACTS})

# subdirectories
add_subdirectory(libs)
if (WITH_TESTS)
    add_subdirectory(tests)
endif()
if (WITH_APPS)
    add_subdirectory(apps)
endif()
