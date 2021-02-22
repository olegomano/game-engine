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
  
  start_color();
  init_pair(COLOR_BLACK_INDEX, COLOR_BLACK, COLOR_BLACK);
  init_pair(COLOR_WHITE_INDEX, COLOR_BLACK, COLOR_WHITE);
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
      raytrace::Result result = m_tracer.buffer()[y*m_tracer.width() + x];
      std::stringstream oss;
      if(result.distance == 0){
        oss << ' ';
      }else{
        oss << result.distance;
      }
      int colorIndex = COLOR_BLACK_INDEX;
      if(result.distance > 0){
        colorIndex = COLOR_WHITE_INDEX;
      }
      attron(COLOR_PAIR(colorIndex));
      mvaddch(y,x,oss.str()[0]);
      attroff(COLOR_PAIR(colorIndex));
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
    out << "Transform: " << glm::to_string(node.transform().transform()) << std::endl;
    
    if(node.hasPayload()){  
      for(int i = 0; i < depth; i++){
        out << " ";
      }
      out << "Name: " << node.data()->name() << std::endl;
      for(int i = 0; i < depth; i++){
        out << " ";
      }
      out << "Layers:[";
      
     render::asset::Mesh::VertexLayer::for_each([&](typename render::asset::Mesh::VertexLayer::id layerName){
        if(node.data()->hasLayer(layerName)){
          out << render::asset::Mesh::VertexLayer::layer_name(layerName) << ", ";
        }
      });
      out << "]" << std::endl;
      out << "Textures:[";
      render::asset::Mesh::TextureLayer::for_each([&](typename render::asset::Mesh::TextureLayer::id name){
        if(node.data()->hasTexture(name)){
          out << render::asset::Mesh::TextureLayer::layer_name(name) << ", ";
        }
      });
      out << "]";
      out << std::endl << std::endl;
    }
    return ++depth; 
  },0);
}

::render::SceneItem NCursesRender::addMesh(MeshInstance& asset){
  auto scene = asset;
  collections::scene_graph::node_ref ref = m_scene.append(scene);  
  displayScene(debug::print::stream); 
   
  ::render::SceneItem result(Asset(m_scene,ref));
  return result;
}


NCursesRender::~NCursesRender(){
  getch();
  endwin();
}

