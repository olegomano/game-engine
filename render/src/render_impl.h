#pragma once
#include <render/render.h>
#include <asset_import/asset.h>
#include <collections/scene_graph.h>
#include <asset_import/mesh.h>

namespace render{

typedef collections::scene_graph::Scene<const asset::Mesh*> MeshInstance;
typedef MeshInstance::node_type MeshInstanceNode;

class IRenderImpl{
public:
  virtual void render() = 0;
  SceneItem addMesh(MeshInstance& mesh){
    return addMesh(mesh, "");
  }
  virtual SceneItem addMesh(MeshInstance& mesh, const std::string& uuid) {return{};};
  virtual Camera addCamera(){return{};};
  virtual ~IRenderImpl(){}
};
}
