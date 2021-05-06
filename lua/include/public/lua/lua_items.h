#pragma once
#include "lua_table.h"
#include <vector>

namespace lua{

template<>
struct Item<uint32_t>{
  static void read(lua_State* lua, uint32_t& out){
    double number = lua_tonumber(lua, -1);
    out = (uint32_t) number;
    lua_pop(lua,1);
  }

  static void write(lua_State* lua, const uint32_t& out){
    lua_pushnumber(lua, out);
  }
};

template<>
struct Item<float>{
  static void read(lua_State* lua, float& out){
    double number = lua_tonumber(lua, -1);
    out = (float) number;
    lua_pop(lua,1);
  }

  static void write(lua_State* lua, const float& out){
    lua_pushnumber(lua, out);
  }
};


template<>
struct Item<std::string>{
  static void read(lua_State* lua, std::string& in){
    size_t string_len = 0;
    const char* str = lua_tolstring(lua, -1, &string_len);
    in = std::string(str,string_len);
  }

  static void write(lua_State* lua, const std::string& out){
    
  }
};


template<typename T>
struct Item<std::vector<T>>{
  static void read(lua_State* lua, std::vector<T>& out){
    T tmp = {};
    lua_pushnil(lua);
    while(lua_next(lua, -2) != 0){
      Item<T>::read(lua,tmp);
      out.push_back(tmp);
    }
  }

  static void write(lua_State* lua, const std::vector<T>& out){
    int index = 1;
    for(const auto&  item : out){
      Item<T>::write(lua,item);
      lua_rawseti(lua, -2,index++);
    }
  }
};

}


