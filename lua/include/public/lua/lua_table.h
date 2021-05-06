#pragma once
#include <cinttypes>
#include <string>
#include <unordered_map>
extern "C" {
  #include <lua.h>
  #include <lauxlib.h>
  #include <lualib.h>
}

namespace lua{

template<typename T>
struct Item{
  static void read(lua_State* lua, T& out) = delete;
  static void write(lua_State* lua, const T& item) = delete;
  typedef void(*read_f)(lua_State* , T& );
  typedef void(*write_f)(lua_State* , const T& );
};


class Table{
public:
  Table(lua_State* state, const std::string& name){
    m_name  = name;
    m_state = state;
    lua_newtable(m_state);
    lua_setglobal(state, name.c_str());
  }

  Table(Table&& other){
    m_name = std::move(other.m_name);
    m_state = other.m_state;
  }

  Table(const Table& other){
    m_name = other.m_name;
    m_state = other.m_state;
  }
  
  template<typename T>
  void get(const std::string& name, T& out){
    lua_getglobal(m_state, m_name.c_str()); 
    lua_pushstring(m_state, name.c_str());
    lua_gettable(m_state,-2);  
    Item<T>::read(m_state,out);
    lua_pop(m_state, 1);
  }

  template<typename T>
  bool set(const std::string& name, const T& data){
    lua_getglobal(m_state, m_name.c_str());
    lua_pushstring(m_state,name.c_str());
    lua_newtable(m_state);
    Item<T>::write(m_state,data);  
    lua_settable(m_state, -3);
    lua_pop(m_state, 1);
    return true;
  }
  
  /*
   * Can't be used with primtive types
   * write for T must do call settable internally
   */
  template<typename T>
  bool set(const T& data){
    lua_getglobal(m_state, m_name.c_str());
    Item<T>::write(m_state,data);  
    lua_pop(m_state, 1);
    return true;
  }

  const std::string& name() const {
    return m_name;
  }
private: 
  std::string m_name;
  lua_State*  m_state;
};

}
