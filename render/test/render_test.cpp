#include <render/render.h>
#include<chrono>
#include<thread>

int main(){
  render::RenderContext c;
  c.create(render::RenderContext::TEXT);
  while(true){
    c.render();
    std::this_thread::sleep_for(std::chrono::milliseconds(32));
  }
  return 1;
}
