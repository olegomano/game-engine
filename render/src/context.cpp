#include <window.h>
#include <imgui.h>
#include <render_impl.h>
#include <gl_impl/gl_impl.h>
#include <ui_helpers.h>
#include <ncurses_impl/ncurses_impl.h>
#include <software_impl/software_impl.h>
#include <asset_import/asset.h>
#include <ostream>
#include <iostream>
#include <ui_manager.h>
#include <render/render.h>
#include <collections/observable.h>

render::RenderContext::RenderContext() = default;
render::RenderContext::~RenderContext() = default;

void render::RenderContext::create(render::RenderContext::Type t){
  
  switch(t){
    case GL_3:
      {
      m_impl = std::make_unique<gl::GLRender>();
      }
      break;
    case TEXT:
      {
      m_impl  = std::make_unique<ncurses::NCursesRender>();
      }
      break;
    case SOFTWARE:
      {
      render::window::params params = {1024,1024,"SoftwareRenderer"};
      m_window  = std::make_unique<window::GLES2Window>(params);
      m_impl    = std::make_unique<software::SoftwareRenderer>();
      m_uiManager = std::make_unique<UiManager>(*this);

      m_window->addRenderer(std::bind(&software::SoftwareRenderer::render,(software::SoftwareRenderer*)m_impl.get()));
      addUiTab("Assets",std::bind(&UiManager::drawAssetInspectorUI,m_uiManager.get()));
      addUiTab("Scene",std::bind(&software::SoftwareRenderer::drawSceneInspectorUI,(software::SoftwareRenderer*)m_impl.get()));
      addUiTab("Console",std::bind(&UiManager::drawConsoleUI,m_uiManager.get()));
      
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

render::SceneItem render::RenderContext::loadAsset(const std::string& v){
  using namespace render::asset;
  std::cout << "RenderContext::createAsset " << v << std::endl; 
  SceneAsset* asset = m_assetManager.load(v);
  if(asset == nullptr){
    std::cout << "RenderContext::createAsset failed to create" << v << std::endl; 
  }else{
    //std::cout << "RenderContext::createAsset " <<  std::endl << *asset << std::endl; 
    MeshInstance instance = asset->instance();
    render::SceneItem item =  m_impl->addMesh(instance,v);
    collections::Observable<render::RenderContext,render::SceneItem>::push_back(item);
    return item;
  }
  return {};
}

void render::RenderContext::onItemAdded(const render::SceneItem& item){

}

void render::RenderContext::onItemAdded(const render::Camera& camera){

}


render::Camera render::RenderContext::createCamera(){  
  return {};
}

render::window::IWindow* render::RenderContext::window(){
  return m_window.get();
}
void render::RenderContext::addInputHandler(const render::window::InputHandler& listener){
  m_window->addInputHandler(listener);
}
void render::RenderContext::addEventHandler(const render::window::EventHandler& handler){
  m_window->addEventHandler(handler);
}

void render::RenderContext::addUiTab(const std::string& name, const render::window::UiTab& tab){
  m_window->addUiTab(name,tab);
}

void render::RenderContext::addUiWindow(const std::string& name, const render::window::UiWindow& window){
  m_window->addUiWindow(name, window);
}

void render::RenderContext::render(){
  m_window->render();
  m_window->pollInput();
}

