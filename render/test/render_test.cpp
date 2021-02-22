#include <render/render.h>
#include <collections/debug_print.h>
#include <collections/timer.h>
#include<chrono>
#include<thread>
#include<glm/gtc/matrix_transform.hpp>

int main(){
  render::RenderContext c;
  c.create(render::RenderContext::SOFTWARE); 
  auto plane = c.loadAsset("../../assets/TIE.dae");
  //auto plane_2 = c.createAsset("../../assets/plane.dae");
  //auto plane = c.createAsset("../../assets/cube.dae");
  bool running = true;
  
  c.input()->addInputListener([&](uint32_t key_int){
    char key = (char)key_int;
    debug::print::print_debug("Pressed Key ", key);
    if(key == 'W'){
      plane->transform().translate(0.1,0,0);
    }else if(key == 'S'){
      plane->transform().translate(-0.1,0,0);
    }
    if(key == 'A'){
      plane->transform().rotate(0.05f,glm::vec3(-1,0,0));
    }else if(key == 'D'){ 
      plane->transform().rotate(0.05f,glm::vec3(1,0,0));
    }
    else if(key == 'Q'){
      plane->transform().rotate(0.05f,glm::vec3(0,1,0));
    }else if(key == 'E'){
      plane->transform().rotate(-0.05f,glm::vec3(0,1,0));
    }
  });


  c.input()->addEventListener([&](render::IInputManager::event e){
    if( e == render::IInputManager::Quit){
      running = false;
    }
  });

  while(running){
    debug::print::print_debug("=================");
    auto timer = debug::timer::ImmidiateTimer([&](){
      c.render();
    });
    debug::print::print_debug("Render Time: ", timer.time() ,"ms");

    std::this_thread::sleep_for(std::chrono::milliseconds(16));
  }
  return 1;
}
