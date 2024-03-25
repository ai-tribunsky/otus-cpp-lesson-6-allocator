cmake_minimum_required(VERSION 3.16)

project(
    allocator
    VERSION 0.0.0
    DESCRIPTION "Custom allocator for STL containers"
    LANGUAGES CXX
)

configure_file(include/version.h.in include/version.h)
add_executable(${CMAKE_PROJECT_NAME} src/main.cpp)

# add targets include dirs
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE include ${CMAKE_BINARY_DIR}/include)

# set targets C++ standard
set_target_properties(
    ${CMAKE_PROJECT_NAME}
    PROPERTIES CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON)

# enable compile warnings
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wshadow -Werror)

install(TARGETS ${CMAKE_PROJECT_NAME} RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT ai.tribunsky@gmail.com)
include(CPack)

# testing
if(BUILD_TESTING)
    enable_testing()

    # fetch Google Test framework
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    FetchContent_MakeAvailable(googletest)

    add_executable(${CMAKE_PROJECT_NAME}_test test/otus_allocator_test.cpp)

    target_include_directories(${CMAKE_PROJECT_NAME}_test PRIVATE include)

    target_link_libraries(
        ${CMAKE_PROJECT_NAME}_test
        GTest::gtest_main
    )
    include(GoogleTest)
    gtest_discover_tests(${CMAKE_PROJECT_NAME}_test)

    set_target_properties(
        ${CMAKE_PROJECT_NAME}_test
        PROPERTIES CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON)

    target_compile_options(
        ${CMAKE_PROJECT_NAME}_test
        PRIVATE -Wall -Wextra -Wpedantic -Wshadow -Werror)

    add_test(
        NAME ip_filter_test
        COMMAND ${CMAKE_PROJECT_NAME}_test
    )
endif()
