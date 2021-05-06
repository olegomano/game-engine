cmake_minimum_required(VERSION 3.5)
project(lua)

add_custom_command(OUTPUT liblua.a
                   COMMAND make all 
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lua
)


add_custom_target(lua_target DEPENDS liblua.a)


add_library(lib_lua STATIC IMPORTED GLOBAL)
add_dependencies(lib_lua lua_target)

set_target_properties(lib_lua 
  PROPERTIES 
  IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lua/liblua.a
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/lua/
  LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/lua/liblua.a
)
target_link_libraries(lib_lua INTERFACE dl)

