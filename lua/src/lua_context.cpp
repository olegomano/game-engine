#include <lua/lua_context.h>
#include <lua_context_impl.h>

using namespace lua;

LuaContext::LuaContext(const std::string& dir){
  m_working_dir = dir;  
  m_impl = std::make_unique<LuaContextImpl>(); 
}

bool LuaContext::executeString(const std::string& str, std::string& error){
  return m_impl->executeString(str,error);
}

void LuaContext::setPrintHandler(const lua::LuaPrint& print){
  m_impl->setPrintHandler(print);
}

lua_State* LuaContext::lua(){
  return m_impl->lua();
}

LuaContext::~LuaContext(){
  
}


