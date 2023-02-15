cmake_minimum_required(VERSION 3.24)
project(CPPDSB)

set(CMAKE_CXX_STANDARD 23)

find_package(Git QUIET)
if (GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if (GIT_SUBMODULE)
        message(STATUS "Submodule updated")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE GIT_SUBMODULE_RESULT)
        if (NOT GIT_SUBMODULE_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/curlcpp/CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

add_subdirectory(curlcpp)
include_directories(${CURLCPP_SOURCE_DIR}/include)

add_library(CPPDSB library.cpp)
target_link_libraries(CPPDSB PUBLIC curlcpp fmt curl jsoncpp)
#target_include_directories(CPPDSB PUBLIC "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/curlcpp")