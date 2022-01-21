cmake_minimum_required(VERSION 3.16)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# CMake Modules Path
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

## Project Name ##
project(FreeRTOS-Cpp)

## FreeRTOS-Cpp Interface ##
set(FREERTOS_CPP_PATH ${CMAKE_CURRENT_LIST_DIR}/FreeRTOS-Cpp)
add_subdirectory(${FREERTOS_CPP_PATH})

## FreeRTOS Kernel Config ##
# Include the FreeRTOS kernel and configure it for the ARM M0 port so the examples will compile.
set(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_LIST_DIR}/FreeRTOS-Kernel)
set(FREERTOS_KERNEL_PORT ARM_CM0)
set(FREERTOS_KERNEL_CONFIG ${CMAKE_CURRENT_LIST_DIR}/examples/config)
set(FREERTOS_KERNEL_HEAP_FILE heap_4)
include(FreeRTOS-Kernel)

target_compile_options(FreeRTOS-Kernel PUBLIC
    "-mcpu=cortex-m0"
    "-fno-exceptions"
)


## Examples' Configuration ##
add_custom_target(example-all)

file(GLOB_RECURSE EXAMPLE_SOURCES -CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/*/*.cpp"
)

# Create a target for each example file as an object library to ensure that it compiles.
foreach(SOURCE_FILE ${EXAMPLE_SOURCES})
    get_filename_component(Filename ${SOURCE_FILE} NAME_WLE)
    get_filename_component(Directory ${SOURCE_FILE} DIRECTORY)
    get_filename_component(Directory ${Directory} NAME)
    string(TOLOWER ${Directory} Directory)
    string(CONCAT Example "example-" ${Directory} "-" ${Filename})
    add_library(${Example} OBJECT ${SOURCE_FILE})
    target_link_libraries(${Example}
        FreeRTOS-Cpp
    )
    add_dependencies(example-all ${Example})
endforeach()


## Doxygen Configuration ##
find_program(DOXYGEN "doxygen")
if (DOXYGEN)
    add_custom_target( doxygen
        COMMAND doxygen ${CMAKE_HOME_DIRECTORY}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
    )
endif()


## Clang-Tidy Configuration ##
find_program(CLANG_TIDY "clang-tidy")
if (CLANG_TIDY)
    file(GLOB_RECURSE CLANG_TIDY_FILES -CONFIGURE_DEPENDS
        "${FREERTOS_CPP_PATH}/include/*"
    )

    add_custom_target( clang-tidy
        COMMAND clang-tidy
        ${CLANG_TIDY_FILES}
        -p ${CMAKE_HOME_DIRECTORY}/build
        --
        -std=c++17
        -I ${FREERTOS_CPP_PATH}/include
        -I ${FREERTOS_KERNEL_CONFIG}
        -I ${FREERTOS_KERNEL_PATH}/include
        -I ${FREERTOS_KERNEL_PORT}
    )
endif()


## Clang-Format Configuration ##
find_program(CLANG_FORMAT "clang-format")
if (CLANG_FORMAT)
    file(GLOB_RECURSE CLANG_FORMAT_FILES -CONFIGURE_DEPENDS
        "${FREERTOS_CPP_PATH}/include/*"
        "${CMAKE_CURRENT_SOURCE_DIR}/examples/*/*.cpp"
    )

    # Target to run Clang-Format and fix errors.
    add_custom_target( clang-format
        COMMAND clang-format
        -Werror
        -i
        -style=file
        ${CLANG_FORMAT_FILES}
    )

    # Target to run Clang-Format and report errors without making changes to files.
    add_custom_target( clang-format-dry-run
        COMMAND clang-format
        -dry-run
        -Werror
        -i
        -style=file
        ${CLANG_FORMAT_FILES}
    )
endif()
