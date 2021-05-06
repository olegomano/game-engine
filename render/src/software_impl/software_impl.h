#pragma once
#include <memory.h>
#include <render/render.h>
#include <render_impl.h>
#include <window_impl.h>
#include <asset_import/asset.h>
#include <collections/ring_buffer.h>
#include <shaders/flat_color.h>
#include <gl_utils/vbo.h>
#include <string>

namespace render{
namespace software{ 

class SoftwareRenderer;
class DebugUIHandler;

class SceneItemImpl : public ::render::ISceneItemFunctions{
public:
  SceneItemImpl(collections::scene_graph::node_ref index, SoftwareRenderer* owner, const std::string& uri);
  
  TransformWrapper transform() const override;
  uint32_t uuid() const override;
  const std::string& uri() const override;
  //TransformWrapper transform(const std::string& name) const override;
private:
  collections::scene_graph::node_ref m_node;
  SoftwareRenderer* m_owner = nullptr;
  std::string m_uri;
};

class CameraImpl : public Camera{
  
};

struct Renderable{
  std::string name;
  gl::VBO vertex_vbo;
};

class SoftwareRenderer : public IRenderImpl{ 
public:
  friend SceneItemImpl;
  friend CameraImpl;
  typedef collections::scene_graph::Scene<Renderable> Scene;

  SoftwareRenderer();
  void render() override;
  void drawSceneInspectorUI();
  ::render::SceneItem addMesh(render::MeshInstance& mesh, const std::string& uuid) override;
  ::render::Camera addCamera() override;
protected:
  render::gl::VBO handleMesh(const render::asset::Mesh* mesh); 
private:
  Scene m_scene; 
  std::vector<collections::scene_graph::node_ref> m_sceneItems;

  collections::ring_buffer::RingBuffer<collections::ring_buffer::ArrayWrapper<float,128>> m_frameRate;

  render::shader::flat_color::flat_color m_colorShader;
  render::gl::VBO m_triangleVBO; 
};

}
}
