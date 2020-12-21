#pragma once
#include <render/render.h>
#include <render_impl.h>
#include "ray_tracer.h"
#include <collections/scene_graph.h>
#include <memory>
#include <curses.h>
#include <vector>


namespace render{
namespace ncurses{

class Asset;

class InputManager : public IInputManager{
public:
  InputManager();
  void handleKeyboard(uint32_t key);
  void pollInput();
};

class NCursesRender : public IRenderImpl{
public:
  friend Asset;
  typedef collections::scene_graph::Scene<const render::asset::Mesh*> SceneGraph;

  NCursesRender();
  void render() override;
  ::render::IAsset addAsset(::render::asset::SceneAsset& asset) override;

  void displayScene(std::ostream& out);

  ~NCursesRender();
private:
  void addMesh(::render::asset::Mesh* m);

private:
  uint32_t m_count = 0;
  SceneGraph m_scene;
  raytrace::RayTracer m_tracer = raytrace::RayTracer(128,64);
};

class Asset : public ::render::IAssetFunctions{
public:
  Asset(const Asset& other);
  Asset(typename NCursesRender::SceneGraph& graph, collections::scene_graph::node_ref node);
  ~Asset(); 
  const glm::mat4& transform() const override;
  glm::mat4& transform() override;
private:
  collections::scene_graph::node_ref m_node;
  typename NCursesRender::SceneGraph& m_graph;
};

}
}
