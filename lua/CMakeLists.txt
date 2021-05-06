cmake_minimum_required(VERSION 3.5)
project(lua)

SET(LIB_SRC src/lua_context.cpp)
SET(LIB_LINKED_LIBS lib_lua)
add_library(lua_context STATIC ${LIB_SRC})
target_include_directories(lua_context PUBLIC include/public/)
target_include_directories(lua_context PRIVATE include/)
target_link_libraries(lua_context PUBLIC ${LIB_LINKED_LIBS})

add_executable(lua_repl test/repl.cpp)
target_link_libraries(lua_repl lua_context )

set_target_properties(lua_repl PROPERTIES
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF
            )


set_target_properties(lua_context PROPERTIES
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF
            )


