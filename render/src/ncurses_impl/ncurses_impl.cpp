#include "ncurses_impl.h"

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

void NCursesRender::render(){
  erase();
  ++m_count;
  for(int x  = 0; x < COLS - 1; x++){
    for(int y = 0; y < LINES - 1; y++){
      mvaddch(y,x,'*');
    }
  }

  for(auto& asset : m_assets){
    for(auto& mesh: asset->meshes()){
      for(auto& face : mesh.faces<::render::asset::Mesh::vertex>()){
        const glm::vec4& v1 = face[0];
        const glm::vec4& v2 = face[1];
        const glm::vec4& v3 = face[2];

      }
    }
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

