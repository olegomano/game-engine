#include "software_impl.h"
#include "ui_helpers.h"
#include <imgui.h>
#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl2.h>
#include <collections/timer.h>
#include <glm/gtc/type_ptr.hpp>

render::software::SceneItemImpl::SceneItemImpl(collections::scene_graph::node_ref index, SoftwareRenderer* owner, const std::string& uri) : 
  m_node(index),
  m_owner(owner),
  m_uri(uri)
{}

render::ISceneItemFunctions::TransformWrapper render::software::SceneItemImpl::transform() const {
  return render::ISceneItemFunctions::TransformWrapper( &m_owner->m_scene[m_node].transform() ); 
};


uint32_t render::software::SceneItemImpl::uuid() const {
  return m_node.index;
}

const std::string& render::software::SceneItemImpl::uri() const {
  return m_uri;
}

static constexpr float triangle[] = {0,0,0,1,
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
  
  m_scene.update();

  for(const auto& item : m_scene.globals()){
    const glm::mat4& global_position = std::get<glm::mat4>(item);
    const render::software::Renderable& renderable = std::get<const render::software::Renderable>(item);
      
    m_colorShader.setUniform_uCamera(glm::value_ptr(global_position));
    m_colorShader.setUniform_uModelMatrix(glm::value_ptr(mat));
    m_colorShader.setAttribute_aPosition(renderable.vertex_vbo,0);
    glDrawArrays(GL_TRIANGLES,0,renderable.vertex_vbo.size() / (sizeof(float) * 4));
    //lDrawArrays(GL_TRIANGLES,);
    renderable.vertex_vbo.unbind();
  } 
  m_colorShader.disable();
}


void render::software::SoftwareRenderer::drawSceneInspectorUI(){
  using render::software::SoftwareRenderer;
  int treenode_id = 0;
  
  if(ImGui::TreeNode("Scene")){
    
    for(auto tuple : m_scene.globals()){
      const glm::mat4& transform = std::get<glm::mat4>(tuple);
      const Scene::GlobalPositionType& node = std::get<Scene::GlobalPositionType>(tuple); 
      
      if(!node.name.empty()){
        if(ImGui::TreeNode((const void*)treenode_id++,"%s",node.name.c_str())){
          render::ui<glm::mat4>::draw(transform);
          ImGui::TreePop();
        }
      }
    }
    ImGui::TreePop();
  }

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


render::gl::VBO render::software::SoftwareRenderer::handleMesh(const render::asset::Mesh* mesh){
  render::gl::VBO result;
  if(mesh->hasLayer<render::asset::Mesh::VertexLayer::vertex>()){
    result.allocateRaw(
        mesh->layerVertexStart<render::asset::Mesh::VertexLayer::vertex>(), 
        mesh->vertexCount() * 4 * sizeof(float) ); 
  }
  return result;
} 

render::SceneItem render::software::SoftwareRenderer::addMesh(render::MeshInstance& instance, const std::string& uri){

  collections::scene_graph::Scene<render::software::Renderable> vbo_scene;
  instance.morph_into<render::software::Renderable>(vbo_scene, 
      [&](const render::MeshInstance::data_type& mesh ){
        render::software::Renderable r;
        r.vertex_vbo = handleMesh(mesh); 
        r.name = mesh->name();
        return r;
      }
  );
  collections::scene_graph::node_ref mesh_ref = m_scene.append(vbo_scene);
  m_sceneItems.push_back(mesh_ref);

  SceneItemImpl impl(mesh_ref,this,uri);
  render::SceneItem item(impl);
  return item;
}

render::Camera render::software::SoftwareRenderer::addCamera(){
  return {};
}


