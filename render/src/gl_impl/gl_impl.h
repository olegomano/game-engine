#pragma once
#include <render/render.h>
#include <render_impl.h>

namespace render{
namespace gl{

class GLRender : public IRenderImpl{
public:
  ::render::SceneItem addMesh(collections::scene_graph::Scene<const asset::Mesh*> mesh) {return{};};
  ::render::Camera addCamera(){return{};}
  void render(){}
};

}
}
