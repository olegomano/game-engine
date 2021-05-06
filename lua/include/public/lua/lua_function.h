#pragma once
#include <functional>
#include <tuple>
#include <sstream>
#include <iostream>
#include <type_traits>
extern "C" {
  #include <lua.h>
  #include <lauxlib.h>
  #include <lualib.h>
}
#include "lua_items.h"

namespace lua{

template<char ...T>
struct const_str{  
  static std::string as_string(){
    std::stringstream ss;
    ((ss << T),...);
    return ss.str();
}
};

template<typename First, typename ...T>
struct reverse_tuple{
  typedef decltype( std::tuple_cat< reverse_tuple<T...>::value, std::tuple<First>>() ) value; 
};


template<typename Last>
struct reverse_tuple<Last>{
  typedef std::tuple<Last> value;
};


template<uint32_t ID, typename ...T>
class Function{
  typedef Function<ID,T...> this_type;
  typedef std::function<int(T...)> function_type;
public:
  Function(lua_State* lua, const std::string& name, function_type f){
    m_function = f;
    m_state = lua;
    m_name = name;
    lua_pushcfunction(lua, l_func);
    lua_setglobal(lua, name.c_str());
    lua_pushlightuserdata(lua, this);
    lua_setglobal(lua, userdataName().c_str());
  }


private:
  static std::string userdataName() {
    std::stringstream stream;
    stream << ID << "_function_userdata";
    return stream.str();
  }
  
  static int l_func(lua_State* state){
    //for int t in ...T get
    //typename reverse_tuple<T...>::value tuple;
    std::tuple<T...> tuple;
    std::apply([&](auto&&... args) {
        ((Item<typename std::decay<decltype(args)>::type>::read
          ( state, args ) 
        ),...);}, tuple);

    lua_getglobal(state,userdataName().c_str());
    this_type* function_wrapper =  (this_type*) lua_touserdata(state, -1);
    function_type& function = function_wrapper->m_function;
    lua_pop(state, 1);

    //function( ( std::get<T>(tuple) )...); typename T
    int32_t return_count;
    std::apply([&](auto&&...args){
        return_count = function( ( args  )... );
    },tuple);

    return return_count;
  }

  function_type m_function;
  lua_State* m_state;
  std::string m_name;
};


}
