#include <render/render.h>
#include <collections/debug_print.h>
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
      plane.transform()[3][2] += 1;
    }else if(key == 's'){
      plane.transform()[3][2] -= 1;
    }
    if(key == 'a'){
      plane.transform()[3][1] += 1;
    }else if(key == 'd'){
      plane.transform()[3][1] -= 1;
    }
    else if(key == 'q'){
      plane.transform() = glm::rotate(plane.transform(),.10f,glm::vec3(0,1,0));
    }else if(key == 'e'){
      plane.transform() = glm::rotate(plane.transform(),-.10f,glm::vec3(0,1,0));
    }
  });

  while(true){
    c.render();
    std::this_thread::sleep_for(std::chrono::milliseconds(16));
  }
  return 1;
}
