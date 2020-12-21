#include "ncurses_impl.h"
#include "ray_tracer.h"
#include <glm/gtx/string_cast.hpp>
#include <collections/debug_print.h>
#include <sstream>

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
  if(c != -1){
    handleKeyboard(c);
  }
}

NCursesRender::NCursesRender(){
  initscr();
  cbreak();
  noecho();
  curs_set(false);
  clear();
}

void NCursesRender::render(){
  erase();
  ++m_count;
  m_scene.update();
  
  auto& globals = m_scene.globals();
  //debug::print::print_debug("Render List Size: ",globals.size());

  m_tracer.render(globals);
  for(int x  = 0; x < m_tracer.width(); x++){
    for(int y = 0; y < m_tracer.height(); y++){
      std::stringstream oss;
      oss << m_tracer.buffer()[y*m_tracer.width() + x];
      mvaddch(y,x,oss.str()[0]);
    }
  }
  //m_tracer.displayBuffer(debug::print::printFile);

  refresh();
}

void NCursesRender::displayScene(std::ostream& out){
  m_scene.traverse([&](const auto& node,int depth) {
    for(int i = 0; i < depth; i++){
      out << " ";
    }
    out << "Transform: " << glm::to_string(node.transform()) << std::endl;
    
    if(node.hasPayload()){  
      for(int i = 0; i < depth; i++){
        out << " ";
      }
      out << "Name: " << node.data()->name() << std::endl;
      for(int i = 0; i < depth; i++){
        out << " ";
      }
      out << "Layers:[";
      for(int i = render::asset::Mesh::LayerType::START + 1; i < render::asset::Mesh::LayerType::END; i++){
        if(node.data()->hasLayer((render::asset::Mesh::LayerType)i)){
          out << i << ",";
        }
      }
      out << "]";
      out << std::endl << std::endl;
    }
    return depth++; 
  },0);
}

::render::IAsset NCursesRender::addAsset(::render::asset::SceneAsset& asset){
  auto scene = asset.scene();
  collections::scene_graph::node_ref ref = m_scene.append(scene);  
  displayScene(debug::print::stream); 
  
  ::render::IAsset result;
  result.create(Asset(m_scene,ref));
  return result;
}


NCursesRender::~NCursesRender(){
  getch();
  endwin();
}

