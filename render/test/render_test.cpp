#include <render/render.h>
#include <render/ui.h>
#include <collections/debug_print.h>
#include <collections/timer.h>
#include <collections/pipe.h>
#include <lua/lua_context.h>
#include<chrono>
#include<thread>
#include<glm/gtc/matrix_transform.hpp>

#include"../src/ui_utils/repl_console.h"

template<>
struct render::ui<render::ReplConsole> {
  static void draw(render::ReplConsole& console){
    std::stringstream str;
    for(const auto& line : console.out()){
      str << line << std::endl;
    }
    
    if(ImGui::BeginChild("",ImVec2(ImGui::GetWindowWidth(),ImGui::GetWindowHeight() - 85),false)){
      ImGui::TextUnformatted(str.str().c_str());
      ImGui::EndChild();
    }
    
    char input_buffer[128];
    strcpy(input_buffer,console.line().c_str());
    ImGui::InputText("Input",input_buffer,IM_ARRAYSIZE(input_buffer));
    console.setLine(std::string(input_buffer));

    if(console.poll() == render::ReplConsole::READY_FOR_INPUT){
      ImGui::SameLine();
      if(ImGui::Button("run")){
        console.execute();
      }
    }
  }
};

class LuaSharedState : public collections::Handler<LuaSharedState,render::SceneItem> {
public:
  LuaSharedState(render::RenderContext& c){
    CONNECT(c,*this,render::SceneItem);
  }
};

int main(){
  render::RenderContext c;
  lua::LuaContext lua;
  collections::task_manager::TaskManager<std::string,std::string> lua_command_manager;
  render::ReplConsole lua_console(lua_command_manager);
  

  lua.setPrintHandler([&](const std::string& str){
    std::cout << "Handled Print: " <<  str << std::endl; 
    lua_console.out().push_back(str);
  });

  lua.registerFunction("load_asset", lua::wrap<std::string>( [&](std::string str){
    lua_console.out().push_back("loading asset: " + str);
    auto asset = c.loadAsset(str);
    lua::Item<uint32_t>::write(lua.lua(),10);
    return 1;
  }));
  
  c.addHandler<render::SceneItem>([&](const render::SceneItem& item, bool added){
    
  });


  auto draw_lua_console = [&](){
    render::ui<decltype(lua_console)>::draw(lua_console);
  };

  c.create(render::RenderContext::SOFTWARE); 
  c.addUiTab("LuaConsole", draw_lua_console );

  auto plane = c.loadAsset("../../assets/TIE.dae");
  //auto plane_2 = c.createAsset("../../assets/plane.dae");
  //auto plane = c.createAsset("../../assets/cube.dae");
  bool running = true;
  
  c.addInputHandler([&](uint32_t key_int){
    char key = (char)key_int;
    debug::print::print_debug("Pressed Key ", key);
    if(key == 'W'){
      plane->transform()->translate(0,0,0.1);
    }else if(key == 'S'){
      plane->transform()->translate(0,0,-0.1);
    }
    if(key == 'A'){
      plane->transform()->translate(-0.1,0,0);
    }else if(key == 'D'){ 
      plane->transform()->translate(0.1,0,0);
    }
    else if(key == 'Z'){
      plane->transform()->translate(0,0.1,0);
    }
    else if(key == 'X'){
      plane->transform()->translate(0,-0.1,0);
    }
    else if(key == 'Q'){
      plane->transform()->rotate(0.05f,glm::vec3(0,1,0));
    }else if(key == 'E'){
      plane->transform()->rotate(-0.05f,glm::vec3(0,1,0));
    }
  });


  c.addEventHandler([&](render::window::event e){
    if( e == render::window::Quit){
      running = false;
    }
  });

  while(running){
    auto timer = debug::timer::ImmidiateTimer([&](){
      c.render();
    });
    lua_command_manager.runTask([&](const std::string& command){
      std::string error;
      lua.executeString(command,error);
      return error;
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(16));
  }
  return 1;
}
