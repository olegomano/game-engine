#include "ncurses_impl.h"
#include "ray_tracer.h"

using namespace render::ncurses;

InputManager::InputManager(){
  keypad(stdscr,true);
  timeout(0);
}

void InputManager::handleKeyboard(uint32_t t){
  for(const auto& listener : m_handlers){
    listener(t);
  }
}

void InputManager::pollInput(){
  uint32_t c = getch();
  handleKeyboard(c);
}

NCursesRender::NCursesRender(){
  initscr();
  cbreak();
  noecho();
  curs_set(false);
  clear();
}


uint8_t clampColor(float min, float max, float value){
  float d = (value - min) / (max - min);
  return (uint8_t)255 * d;
}

template<typename T>
void imageToFile(T&& image, int w, int h){
  
}

void NCursesRender::render(){
  //erase();
  ++m_count;
  raytrace::RayTracer tracer(32,32); 
  tracer.render(m_assets[0]->meshes());

  for(int x  = 0; x < tracer.width(); x++){
    for(int y = 0; y < tracer.height(); y++){
      if(tracer.buffer()[y*tracer.width() + x] == 0){
        mvaddch(y,x,'*');
      }else{
        mvaddch(y,x,'_');
      }
    }
  }

  for(uint32_t pixel : tracer.buffer()){
    std::cout << pixel << std::endl;
  }

  refresh();
}

NCursesRender::r_handle NCursesRender::addAsset(const ::render::asset::SceneAsset& asset){
  m_assets.push_back(&asset);
  return -1;
}


NCursesRender::~NCursesRender(){
  getch();
  endwin();
}

