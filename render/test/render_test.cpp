#include <render/render.h>
#include <collections/debug_print.h>
#include <collections/timer.h>
#include<chrono>
#include<thread>
#include<glm/gtc/matrix_transform.hpp>

int main(){
  render::RenderContext c;
  c.create(render::RenderContext::TEXT); 
  auto plane = c.createAsset("../../assets/cube.dae");
  

  c.addInputListener([&](uint32_t key){
    debug::print::print_debug("Pressed Key ", key);
    if(key == 'w'){
      plane.transform().translate(0,0,1);
    }else if(key == 's'){
      plane.transform().translate(0,0,-1);
    }
    if(key == 'a'){
      plane.transform().rotate(0.05f,glm::vec3(-1,0,0));
    }else if(key == 'd'){ 
      plane.transform().rotate(0.05f,glm::vec3(1,0,0));
    }
    else if(key == 'q'){
      plane.transform().rotate(0.05f,glm::vec3(0,1,0));
    }else if(key == 'e'){
      plane.transform().rotate(-0.05f,glm::vec3(0,1,0));
    }
  });

  while(true){
    debug::print::print_debug("=================");
    auto timer = debug::timer::ImmidiateTimer([&](){
      c.render();
    });
    debug::print::print_debug("Render Time: ", timer.time() ,"ms");

    std::this_thread::sleep_for(std::chrono::milliseconds(16));
  }
  return 1;
}
