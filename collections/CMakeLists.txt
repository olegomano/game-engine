cmake_minimum_required(VERSION 3.5)
project(collections)

add_library(libcollection STATIC 
  src/src.cpp
)

target_include_directories(libcollection INTERFACE include/)
target_include_directories(libcollection PRIVATE include/)
target_compile_options(libcollection PRIVATE ${COMPILE_FLAGS})
if(BUILD_TESTS)
  #package_add_test(collection_test "test/test.cpp" "libcollection")
endif()


