#pragma once
#include <render/render.h>
#include <asset_import/asset.h>
#include <collections/scene_graph.h>
#include <asset_import/mesh.h>

namespace render{

typedef collections::scene_graph::Scene<const asset::Mesh*> MeshInstance;
class IRenderImpl{
public:
  virtual void render() = 0;
  virtual SceneItem addMesh(MeshInstance& mesh) {return{};};
  virtual Camera addCamera(){return{};};
  virtual ~IRenderImpl(){}
};
}
