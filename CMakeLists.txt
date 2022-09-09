cmake_minimum_required(VERSION 3.20)

project(chip8-proj LANGUAGES CXX)
include(scripts/cmake/base_configuration.cmake)
include(scripts/cmake/compiler_warnings.cmake)

add_subdirectory(third_party)

set(CHIP8_TARGET_NAME chip8)

set(CHIP8_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/chip8.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/chip8.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/shader_sources.inl
)

add_executable(${CHIP8_TARGET_NAME} ${CHIP8_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX source FILES ${CHIP8_SOURCES})

set_target_warnings(${CHIP8_TARGET_NAME})

target_compile_options(${CHIP8_TARGET_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/MP /fp:fast /external:anglebrackets /external:W1>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic>
)

set_target_properties(${CHIP8_TARGET_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${CHIP8_TARGET_NAME} PRIVATE
    glfw
    glw
)
