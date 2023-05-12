cmake_minimum_required(VERSION 3.20)
if(NOT CMAKE_CXX_STANDARD EQUAL 23)
    message(FATAL_ERROR "C++23 is required")
endif()
option(PUSHDOWN_SANITIZE "Enable sanitizers" OFF)
if(PUSHDOWN_SANITIZE)
    message(STATUS "Sanitizers enabled")
    if(WIN32)
        message(WARNING "Sanitizers may fail to link on Windows")
    endif()
    set(CMAKE_CXX_FLAGS
            "-Wall -Wextra -Wno-sign-compare -pedantic -fsanitize=address,leak,undefined -fno-omit-frame-pointer -g"
            )
endif()

add_executable(pushdown pushdown.cpp PushdownFSM.cpp PushdownFSM.h color.hpp)
add_custom_target(copy_pushdown_files)
add_custom_command(TARGET copy_pushdown_files
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/pda.txt ${CMAKE_CURRENT_BINARY_DIR}/pda.txt
        COMMENT "Copying pda.txt"
        VERBATIM
        DEPENDS {CMAKE_CURRENT_SOURCE_DIR}/pda.txt
)
add_dependencies(pushdown copy_pushdown_files)
