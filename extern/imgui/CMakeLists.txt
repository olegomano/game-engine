cmake_minimum_required(VERSION 3.5)
project(imgui)
find_package(SDL2 REQUIRED)

set(LIB_SRC imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp imgui/backends/imgui_impl_sdl.cpp imgui/backends/imgui_impl_opengl2.cpp)
set(LIB_NAME imgui)
set(INCLUDE_PRIVATE
  imgui/
  ${SDL2_INCLUDE_DIRS}
)

set(INCLUDE_INTERFACE
  imgui/
)

set(LINKED_LIBRARIES
  ${SDL2_LIBRARIES}
)


add_library(${LIB_NAME} STATIC 
  ${LIB_SRC}
) 
 
target_include_directories(${LIB_NAME} PRIVATE ${INCLUDE_PRIVATE}) 
target_include_directories(${LIB_NAME} PUBLIC ${INCLUDE_INTERFACE})
target_link_libraries(${LIB_NAME} PUBLIC ${LINKED_LIBRARIES})
target_compile_options(${LIB_NAME} PRIVATE ${COMPILE_FLAGS})
