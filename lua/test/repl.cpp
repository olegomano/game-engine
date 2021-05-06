#include <iostream>
#include <lua/lua_context.h>
#include <lua/lua_items.h>
#include <lua/lua_function.h>
#include <assert.h>


int main(){
  std::cout << "hello world" << std::endl;
  
  

  lua::LuaContext context;
  lua::Table table(context.lua(), "table");
  lua::Function<__COUNTER__,uint32_t,float> 
    function(context.lua(),"func",[](const float i, const uint32_t i2){
      std::cout << i << ", " << i2 << std::endl;
    });

  lua::Function<__COUNTER__,uint32_t,float> 
    function_w(context.lua(),"func_2",[](const float i, const uint32_t i2){
      std::cout << "2: " << i << ", " << i2 << std::endl;
    });

  auto f_3 = [&](float i, uint32_t i2){
    std::cout << "3: " << i << ", " << i2 << std::endl;
  };

  context.registerFunction("func_3",lua::wrap<uint32_t>([&](uint32_t i){
    std::cout << "3:" << std::endl;
  }));


  uint32_t test_int;
  std::vector<uint32_t> test_vector;
  std::vector<std::vector<uint32_t>> test_vector_vector;
  test_vector.push_back(0);
  test_vector.push_back(1);
  test_vector.push_back(2);
  
  test_vector_vector.push_back(test_vector);
  test_vector_vector.push_back(test_vector);

  table.set<uint32_t>("integer",33);
  table.get<uint32_t>("integer",test_int);
  table.set("vector",test_vector);
  table.set("vector_vector", test_vector_vector);
  table.get("vector",test_vector);

  for(const auto& i : test_vector){
    std::cout << i << std::endl;
  }
  assert(test_int==33);

  std::string error = "";
  while(true){
    std::string line;
    std::getline(std::cin,line);
    if(!context.executeString(line,error)){ 
      std::cout << error << std::endl;
    }
  }
  return 0;
}

