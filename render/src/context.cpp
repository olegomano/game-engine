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
#include <render/render.h>

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
      m_window  = std::make_unique<window::GLES2Window>(params);
      m_impl    = std::make_unique<software::SoftwareRenderer>();
      m_input   = std::make_unique<window::SDLInputManager>();
      
      m_window->addRenderer(std::bind(&software::SoftwareRenderer::render,(software::SoftwareRenderer*)m_impl.get()));
      m_window->addUiTab(std::bind(&render::RenderContext::drawAssetInspectorUI,this));
      m_window->addUiTab(std::bind(&software::SoftwareRenderer::drawSceneInspectorUI,(software::SoftwareRenderer*)m_impl.get()));
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

void render::RenderContext::drawAssetInspectorUI(){
  for(const auto& [path,asset] : m_assetManager.assets()){
    if(ImGui::TreeNode(asset->path().c_str())){
      render::ui<std::decay<decltype(asset)>::type>::draw(asset);
      ImGui::TreePop();
    }
  } 
}

render::SceneItem render::RenderContext::loadAsset(const std::string& v){
  using namespace render::asset;
  std::cout << "RenderContext::createAsset " << v << std::endl; 
  SceneAsset* asset = m_assetManager.load(v);
  if(asset == nullptr){
    std::cout << "RenderContext::createAsset failed to create" << v << std::endl; 
  }else{
    //std::cout << "RenderContext::createAsset " <<  std::endl << *asset << std::endl; 
    MeshInstance instance = asset->scene();
    return m_impl->addMesh(instance);
  }
  return {};
}

render::Camera render::RenderContext::createCamera(){
  
  return {};
}

render::window::IWindow* render::RenderContext::window(){
  return m_window.get();
}

render::IInputManager* render::RenderContext::input(){
  return m_input.get();
}


void render::RenderContext::render(){
  m_window->render();
  m_input->pollInput();
}
