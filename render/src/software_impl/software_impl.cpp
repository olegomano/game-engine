#include "software_impl.h"
#include "ui_helpers.h"
#include <imgui.h>
#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl2.h>
#include <collections/timer.h>
#include <glm/gtc/type_ptr.hpp>

render::software::SceneItemImpl::SceneItemImpl(uint32_t index, SoftwareRenderer* owner) : 
  m_index(index),
  m_owner(owner){

}

cgeom::transform::Transform& render::software::SceneItemImpl::transform(){
  assert(m_owner != nullptr);
  assert(m_index != -1);
  return m_owner->m_transforms[m_index]; 
}

const cgeom::transform::Transform& render::software::SceneItemImpl::transform() const {
  assert(m_owner != nullptr);
  assert(m_index != -1);
  return m_owner->m_transforms[m_index]; 
}


static const float triangle[] = {0,0,0,1,
                                 0,1,0,1,
                                 1,0,0,1};

//////////////////SoftwareRenderer
render::software::SoftwareRenderer::SoftwareRenderer(){
    m_colorShader.compile(); 
    m_colorShader.disable();
    m_triangleVBO.allocate(triangle); 
}

static int count = 0;
void render::software::SoftwareRenderer::render(){
  count++;
  glm::mat4 mat(1);
  m_colorShader.enable();
  
  for(size_t i = 0; i < m_models.size(); i++){
    render::gl::VBO& vbo = m_models[i];
    const cgeom::transform::Transform& transform = m_transforms[i];
    
    m_colorShader.setUniform_uCamera(glm::value_ptr(transform.transform()));
    m_colorShader.setUniform_uModelMatrix(glm::value_ptr(mat));
    m_colorShader.setAttribute_aPosition(vbo,0);
    glDrawArrays(GL_TRIANGLES,0,vbo.size() / (sizeof(float) * 4));
    vbo.unbind();
  } 
  m_colorShader.disable();
}


void render::software::SoftwareRenderer::drawSceneInspectorUI(){
  /**
  for(int i = 0; i < m_assets.size(); i++){
    const render::asset::SceneAsset* asset = m_assets[i];
    const cgeom::transform::Transform& transform = m_transforms[i];
    std::string assetName = asset->path() + "_" + std::to_string(i); 

    if(ImGui::TreeNode(assetName.c_str())){
      render::ui<glm::mat4>::draw(transform);
      ImGui::TreePop();
    } 
  }
  **/
}


render::SceneItem render::software::SoftwareRenderer::addMesh(render::MeshInstance& instance){
  m_transforms.push_back(glm::mat4(1.0)); 
  const render::asset::Mesh* const mesh = instance.nodes()[0].data();
  assert(mesh != nullptr);

  render::gl::VBO vbo;
  vbo.allocate(mesh->layer<render::asset::Mesh::VertexLayer::vertex>());
  m_models.push_back(vbo);
  
  render::SceneItem item;
  return item;
}

render::Camera render::software::SoftwareRenderer::addCamera(){
  return {};
}


