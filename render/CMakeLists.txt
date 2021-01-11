cmake_minimum_required(VERSION 3.5)
project(render)
find_package(OpenGL REQUIRED)
find_package(Curses REQUIRED)

set(LIB_SRC
  src/render.cpp 
  src/ncurses_impl/ncurses_impl.cpp 
  src/ncurses_impl/rasterizer.cpp
  src/ncurses_impl/asset.cpp
  src/asset_import/import.cpp
  src/asset_import/texture.cpp
  src/asset_import/asset.cpp
)

set(INCLUDE_PRIVATE
  ${OPENGL_INCLUDE_DIRS}
  ${CURSES_INCLUDE_DIRS}
  ${STB_INCLUDE_DIRS}
  include/
  src/
)

set(INCLUDE_INTERFACE
  include/
)

set(LINKED_LIBRARIES
  ${GLEW_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${CURSES_LIBRARIES}
  assimp
  glm
  cgeom
  collections
)

add_library(librender STATIC 
  ${LIB_SRC}
) 
#add_definitions( -DSTB_IMAGE_IMPLEMENTATION )
target_include_directories(librender PRIVATE ${INCLUDE_PRIVATE}) 
target_include_directories(librender INTERFACE ${INCLUDE_INTERFACE})
target_link_libraries(librender ${LINKED_LIBRARIES})
target_compile_options(librender PRIVATE ${COMPILE_FLAGS})


if(BUILD_TESTS)
  package_add_test(NAME api_test SRC test/test.cpp LIBS librender)
  
  package_add_test(NAME import_test 
    SRC test/import_test.cpp ${LIB_SRC}
    LIBS ${LINKED_LIBRARIES}
    INCLUDE ${INCLUDE_PRIVATE}
    FLAGS ${COMPILE_FLAGS}
    )
  
  add_executable(render_test test/render_test.cpp)
  target_link_libraries(render_test librender)
endif()


