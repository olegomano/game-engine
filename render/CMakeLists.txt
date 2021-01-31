cmake_minimum_required(VERSION 3.5)
project(render)
find_package(OpenGL REQUIRED)
find_package(Curses REQUIRED)
find_package(SDL2 REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)

set(LIB_SRC
  src/render.cpp 
  src/input_handler_impl.cpp
  src/ncurses_impl/ncurses_impl.cpp 
  src/ncurses_impl/rasterizer.cpp
  src/ncurses_impl/asset.cpp
  src/software_impl/software_impl.cpp
  src/asset_import/import.cpp
  src/asset_import/texture.cpp
  src/asset_import/asset.cpp
)

set(INCLUDE_PRIVATE
  ${GLEW_INCLUDE_DIRS}
  ${GLUT_INCUDE_DIRS}
  ${OPENGL_INCLUDE_DIRS}
  ${CURSES_INCLUDE_DIRS}
  ${SDL2_INCLUDE_DIRS}
  ${STB_INCLUDE_DIRS}
  include/
  src/
)

set(INCLUDE_INTERFACE
  include/
)

set(LINKED_LIBRARIES
  ${GLEW_LIBRARIES}
  ${GLUT_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${CURSES_LIBRARIES}
  ${SDL2_LIBRARIES}
  assimp
  glm
  cgeom
  collections
  imgui
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


