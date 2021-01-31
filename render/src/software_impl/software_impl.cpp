#include "software_impl.h"
#include "ui_helpers.h"
#include <imgui.h>
#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl2.h>
#include <collections/timer.h>
#include <glm/gtc/type_ptr.hpp>

render::software::Asset::Asset(uint32_t index, SoftwareRenderer* owner) : m_index(index),m_owner(owner){

}

cgeom::transform::Transform& render::software::Asset::transform(){
  assert(m_owner != nullptr);
  assert(m_index != -1);
  return m_owner->m_transforms[m_index]; 
}

const cgeom::transform::Transform& render::software::Asset::transform() const {
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
  m_colorShader.setUniform_uCamera(glm::value_ptr(mat));
  m_colorShader.setUniform_uModelMatrix(glm::value_ptr(mat));
  m_colorShader.setAttribute_aPosition(m_triangleVBO,0);
  glDrawArrays(GL_TRIANGLES,0,3);
  m_triangleVBO.unbind();
  m_colorShader.disable();
}


render::IAsset render::software::SoftwareRenderer::addAsset(::render::asset::SceneAsset& asset){
  m_assets.push_back(&asset);
  m_transforms.push_back(glm::mat4(1.0)); 
  render::IAsset assetWrapper;
  assetWrapper.create<Asset>( Asset( (uint32_t)(m_assets.size() - 1), this) );
  return assetWrapper;
}




