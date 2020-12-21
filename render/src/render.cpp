#include <render/render.h>
#include <render_impl.h>
#include <gl_impl/gl_impl.h>
#include <ncurses_impl/ncurses_impl.h>
#include <asset_import/asset.h>
#include <ostream>
#include <iostream>

render::RenderContext::RenderContext() = default;
render::RenderContext::~RenderContext() = default;

void render::RenderContext::create(render::RenderContext::Type t){
  switch(t){
    case GL_3:
      m_impl = std::make_unique<gl::GLRender>();
      m_input = std::make_unique<ncurses::InputManager>();
      break;
    case TEXT:
      m_impl  = std::make_unique<ncurses::NCursesRender>();
      m_input = std::make_unique<ncurses::InputManager>();
    default:
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

void render::RenderContext::render(){
  m_impl->render();
  m_input->pollInput();
}
