#pragma once
#include "public/lua/lua_context.h"
#include <stdio.h>
#include <string.h>
#include <string>  
extern "C" {
  #include <lua.h>
  #include <lauxlib.h>
  #include <lualib.h>
}

namespace lua{

class LuaContextImpl{
  static constexpr char* CONTEXT_NAME = "_lua_context_impl";
public:
  LuaContextImpl(){
    m_print_handler = [](const std::string& line){
      std::cout << line << std::endl;
    };
    m_lua_state = luaL_newstate();   /* opens Lua */
    luaL_openlibs(m_lua_state);
    lua_pushlightuserdata(m_lua_state, this);
    lua_setglobal(m_lua_state, CONTEXT_NAME);
    
    lua_pushcfunction(m_lua_state, l_print_handler);
    lua_setglobal(m_lua_state, "print_debug");
    
    //lua_getglobal(m_lua_state, "_G");
    //lua_register(m_lua_state, "print",l_print_handler);
	  //lua_pop(m_lua_state, 1);
  }    

  
  /**
   * Executes the given string
   * returns true on success, false on failure
   * if fails fills error with error
   */
  bool executeString(const std::string& s, std::string& error_string){
    int error = luaL_loadstring(m_lua_state, s.c_str());
    error = lua_pcall(m_lua_state, 0, LUA_MULTRET, 0);
    if (error) {
      error_string = std::string( lua_tostring(m_lua_state, -1) );
      lua_pop(m_lua_state, 1);  /* pop error message from the stack */
      return false;
    }
    return true;
  }
  
  void setPrintHandler(const lua::LuaPrint& handler){
    m_print_handler = handler;
  }

  lua_State* lua(){
    return m_lua_state;
  }

  void print(const std::string& line){
    m_print_handler(line);
  }

  ~LuaContextImpl(){}
private:
  lua_State* m_lua_state;
  LuaPrint   m_print_handler;
 
  static int l_print_handler(lua_State* lua){

    int nargs = lua_gettop(lua);
    for (int i=1; i <= nargs; i++) {
      size_t line_length=0;
      const char* str = lua_tolstring(lua, -1,&line_length);
      std::string line = std::string(str,line_length);

      lua_getglobal(lua, LuaContextImpl::CONTEXT_NAME);    
      LuaContextImpl* context =  (LuaContextImpl*) lua_touserdata(lua, -1);
      context->print(line);   
    }
    return 0;
  }
};



}
