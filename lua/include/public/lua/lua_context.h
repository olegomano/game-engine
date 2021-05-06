#pragma once
#include <memory>
#include <tuple>
#include <optional>
#include <variant>
#include "lua_table.h"
#include "lua_function.h"
#include <unordered_set>
extern "C" {
  #include <lua.h>
  #include <lauxlib.h>
  #include <lualib.h>
}

namespace lua{


template<typename ...T,typename F>
static std::function<uint32_t(T...)> wrap(F f){
  std::function<uint32_t(T...)> func = f;
  return f;
}

typedef std::function<void(const std::string&)> LuaPrint;

class LuaContextImpl;
class LuaContext{
public:
  

  LuaContext(const std::string& working_dir = "./");
  bool executeString(const std::string& str, std::string& error);
  bool loadFile(const std::string& file_name);

  template<typename ...T>
  void registerFunction(const std::string& name , std::function<uint32_t(T...)> f){
    void* func = new Function<__COUNTER__,T...>(lua(),name,f);
    m_func_map[name] = func;
  }
  
  Table& createVar(const std::string& name){
    Table table(lua(),name);
    m_variables.emplace(name, std::move(table));
    return m_variables.at(name); 
  }

  template<typename T>
  Table& createVar(const std::string& name, const T& value){
    Table& t = createVar(name);
    t.set(value);
    return t;
  }

  void setPrintHandler(const LuaPrint& print);

  lua_State* lua();
  ~LuaContext();
private:
  std::unique_ptr<LuaContextImpl> m_impl;
  std::string m_working_dir;
  std::unordered_map<std::string,void*> m_func_map;
  std::unordered_map<std::string,Table> m_variables;
};

}
