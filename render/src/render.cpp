#include <render/render.h>
#include <render_impl.h>
#include <gl_impl/gl_impl.h>
#include <ncurses_impl/ncurses_impl.h>
#include <software_impl/software_impl.h>
#include <asset_import/asset.h>
#include <window.h>
#include <ostream>
#include <iostream>

render::RenderContext::RenderContext() = default;
render::RenderContext::~RenderContext() = default;

void render::RenderContext::create(render::RenderContext::Type t){
  switch(t){
    case GL_3:
      {
      m_impl = std::make_unique<gl::GLRender>();
      m_input = std::make_unique<ncurses::InputManager>();
      }
      break;
    case TEXT:
      {
      m_impl  = std::make_unique<ncurses::NCursesRender>();
      m_input = std::make_unique<ncurses::InputManager>();
      }
      break;
    case SOFTWARE:
      {
      render::window::params params = {1024,1024,"SoftwareRenderer"};
      m_impl = std::make_unique<software::SoftwareRenderer>(params);
      m_input = std::make_unique<window::SDLInputManager>();
      }
      break;
    default:
      assert(false);
      break;
    break;
  }
}

void render::RenderContext::createPrimitive(primitive::Value v){

}

::render::IAsset render::RenderContext::createAsset(const std::string& v){
  using namespace render::asset;
  std::cout << "RenderContext::createAsset " << v << std::endl; 
  SceneAsset* asset = m_assetManager.load<SceneAsset>(v);
  if(asset == nullptr){
    std::cout << "RenderContext::createAsset failed to create" << v << std::endl; 
  }else{
    //std::cout << "RenderContext::createAsset " <<  std::endl << *asset << std::endl; 
    return m_impl->addAsset(*asset);
  }
  return {};
}

void render::RenderContext::addInputListener(const IInputManager::InputHandler& handler){
  m_input->addInputListener(handler);
}

void render::RenderContext::addEventListener(const IInputManager::EventHandler& handler){
  m_input->addEventHandler(handler);
}

void render::RenderContext::addUiHandler(const render::RenderContext::UiDrawHandler& handler){
  m_impl->addUiHandler(handler);
}


void render::RenderContext::render(){
  m_impl->render();
  m_input->pollInput();
}
