cmake_minimum_required(VERSION 3.5)
project(Galacoli)

option(BUILD_TESTS "Build Tests" ON)
option(OPTIMIZATIONS "Are optimizations enabled" OFF)

if(OPTIMIZATIONS)
set(COMPILE_FLAGS "-std=c++17" "-fno-exceptions" "-Wall" "-O3" "-fno-omit-frame-pointer" "-flto" "-fno-rtti" "-Werror=return-type")
else()
set(COMPILE_FLAGS "-std=c++17" "-fno-exceptions" "-Wall" "-Werror=return-type" )
endif()
set(COMPILE_FLAGS "-std=c++17" "-fno-exceptions" "-Wall" "-Werror=return-type" )
if(BUILD_TESTS)
    enable_testing()
    include(GoogleTest)
endif()


macro(package_add_test)
  set(options NOP)
  set(oneValueArgs NAME)
  set(multiValueArgs SRC LIBS INCLUDE FLAGS)
  cmake_parse_arguments(PACKAGE_ADD_TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )


  # create an exectuable in which the tests will be stored
  add_executable(${PACKAGE_ADD_TEST_NAME} ${PACKAGE_ADD_TEST_SRC})
  target_include_directories(${PACKAGE_ADD_TEST_NAME} PRIVATE ${PACKAGE_ADD_TEST_INCLUDE}) 
  target_link_libraries(${PACKAGE_ADD_TEST_NAME} gtest gmock gtest_main ${PACKAGE_ADD_TEST_LIBS})
  target_compile_options(${PACKAGE_ADD_TEST_NAME} PRIVATE ${PACKAGE_ADD_TEST_FLAGS})
  # gtest_discover_tests replaces gtest_add_tests,
  # see https://cmake.org/cmake/help/v3.10/module/GoogleTest.html for more options to pass to it
  gtest_discover_tests(${PACKAGE_ADD_TEST_NAME}
    # set a working directory so your project root so that you can find test data via paths relative to the project root
    WORKING_DIRECTORY ${PROJECT_DIR}
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
  )
  set_target_properties(${PACKAGE_ADD_TEST_NAME} PROPERTIES FOLDER tests)
endmacro()
#find_package(SDL2 REQUIRED)
#find_package(GLEW REQUIRED)


set(SAIL_INCLUDE_DIRS extern/sail/src)
set(STB_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/extern/stb)
add_subdirectory("extern/googletest")
add_subdirectory("extern/assimp")
add_subdirectory("extern/glm")
add_subdirectory("extern/imgui")
add_subdirectory(cgeom)
add_subdirectory(collections)
add_subdirectory(render)


#add_executable(main main.cpp)
#target_link_libraries(main librender)
#target_compile_options(main PRIVATE ${COMPILE_FLAGS})
