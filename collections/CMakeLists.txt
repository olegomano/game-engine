cmake_minimum_required(VERSION 3.5)
project(collections)

set(LIB_SRC "src/debug_print.cpp")

set(LIB_NAME collections)

set(INCLUDE_PRIVATE
  include/
)

set(INCLUDE_INTERFACE
  include/
)
set(LINKED_LIBRARIES
  glm
  cgeom
)


add_library(${LIB_NAME} STATIC 
  ${LIB_SRC}
) 
 
target_include_directories(${LIB_NAME} PRIVATE ${INCLUDE_PRIVATE}) 
target_include_directories(${LIB_NAME} INTERFACE ${INCLUDE_INTERFACE})
target_link_libraries(${LIB_NAME} PUBLIC ${LINKED_LIBRARIES})
target_compile_options(${LIB_NAME} PRIVATE ${COMPILE_FLAGS})


if(BUILD_TESTS) 
  package_add_test(NAME stable_vector 
    SRC test/test.cpp ${LIB_SRC}
    LIBS ${LINKED_LIBRARIES}
    INCLUDE ${INCLUDE_PRIVATE}
    FLAGS ${COMPILE_FLAGS}
  )  
endif()

package_add_bench(NAME sync_que_bench
  SRC perf/sync_que.cpp
  LIBS ${LIB_NAME}
  FLAGS ${COMPILE_FLAGS}
)
