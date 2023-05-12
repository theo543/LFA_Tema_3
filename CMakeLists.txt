cmake_minimum_required(VERSION 3.20)
project(LFA_Tema_3)
set(CMAKE_CXX_STANDARD 23)
option(SANITIZE "Enable sanitizers" OFF)
if(SANITIZE)
    message(STATUS "Sanitizers enabled")
    if(WIN32)
        message(WARNING "Sanitizers may fail to link on Windows")
    endif()
    set(CMAKE_CXX_FLAGS
            "-Wall -Wextra -Wno-sign-compare -pedantic -fsanitize=address,leak,undefined -fno-omit-frame-pointer -g"
            )
endif()
add_executable(CYK CYK.cpp Grammar_FNC.h Grammar_FNC.cpp Array3D.h Array3D.tpp color.hpp)
add_custom_target(copy_cyk_files)
add_custom_command(TARGET copy_cyk_files
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/cyk.txt ${CMAKE_CURRENT_BINARY_DIR}/cyk.txt
        COMMENT "Copying cyk.txt"
        VERBATIM
        DEPENDS {CMAKE_CURRENT_SOURCE_DIR}/cyk.txt
)
add_custom_command(TARGET copy_cyk_files
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/cyk_tests.txt ${CMAKE_CURRENT_BINARY_DIR}/cyk_tests.txt
        COMMENT "Copying cyk_tests.txt"
        VERBATIM
        DEPENDS {CMAKE_CURRENT_SOURCE_DIR}/cyk_tests.txt
)
add_dependencies(CYK copy_cyk_files)
add_executable(recgen recgen.cpp)
