cmake_minimum_required(VERSION 3.11.0)
project(gridanimation VERSION 0.1.0 LANGUAGES C CXX)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_PROGRAM})
endif()

set(CMAKE_CXX_STANDARD 23)
cmake_policy(SET CMP0072 NEW)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic -march=native -fno-omit-frame-pointer)

file(GLOB_RECURSE SOURCES
    CONFIGURE_DEPENDS    
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*.c
)

add_executable(gridanimation ${SOURCES})
target_include_directories(gridanimation PRIVATE ${CMAKE_SOURCE_DIR}/include ${FFTW3_INCLUDE_DIRS})
# target_link_libraries(gridanimation PRIVATE fftw3 pthread asan)
# target_link_options(gridanimation PRIVATE -fsanitize=address)



