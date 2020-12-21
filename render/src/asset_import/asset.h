#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include <variant>
#include <optional>
#include <tuple>
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <collections/scene_graph.h>

namespace render{
namespace asset{

template<typename T>
struct Face{
  std::array<T,3> face;
  
  const T& operator[](uint8_t indx) const {
    assert(indx < 3);
    return face[indx];
  }
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const Face<T>& face){
  os << "[";
  for(const auto& v : face.face){
    os << glm::to_string(v) << ", ";
  }
  os << "]";
  return os;
}

class Mesh{  
public:    
  friend std::ostream& operator<<(std::ostream& os, const Mesh& m);
  enum LayerType{
    START = -1,
    vertex,
    uv_1,
    uv_2,
    normal,
    bone,
    END
  };
  
  template<typename T, LayerType _T_Type>
  struct Layer{
    static constexpr LayerType Type = _T_Type;
    std::optional<std::vector<Face<T>>> faces;
  };

  typedef std::tuple<
    Layer<glm::vec4,vertex>,
    Layer<glm::vec2,uv_1>,
    Layer<glm::vec2,uv_2>,
    Layer<glm::vec4,normal>,
    Layer<glm::vec4,bone>
  >LayerGroup;
  
  template<LayerType T, typename Vector>
  void addLayer(Vector&& vector){
    std::get<T>(m_layers).faces = std::move(vector);
  }  
  
  bool hasLayer(const LayerType& t) const {
    return std::apply([&](auto&... layer){    
      return ( ( (layer.Type == t) && layer.faces.has_value()) || ... );    
    },m_layers);
  }

  template<LayerType T>
  bool hasLayer() const {
    return std::get<T>(m_layers).faces.has_value();
  }
  
  template<LayerType T>
  const auto& layer() const {
    assert(hasLayer<T>());
    return std::get<T>(m_layers).faces.value();
  }

  template<typename Callback>
  void layer(const LayerType& t, Callback&& callback) const {
    std::apply([&](auto&... layer){
      std::make_tuple( dynamicApplyLayer(t,layer,callback) ... );
    },m_layers);
  }
private:
  template<typename Callback, typename Layer>
  bool dynamicApplyLayer(const LayerType& type, const Layer& layer, Callback&& callback) const {
    if(type == layer.Type){
      callback(layer.faces.value());
    }
    return true;
  }


  LayerGroup m_layers;
};

class SceneAsset{
public:    
  friend std::ostream& operator<<(std::ostream& os, const SceneAsset& sa);
  
  SceneAsset(const std::string& path){
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);       
    if(scene == nullptr){
      std::cout << "Failed to open file " << path << std::endl;
      return;
    }
    handleNode(scene->mRootNode,scene,collections::scene_graph::Scene<Mesh>::Root_Ref);
  }

  collections::scene_graph::Scene<const Mesh*> scene(){
    collections::scene_graph::Scene<const Mesh*> scene;
    m_meshes.traverse([&](const collections::scene_graph::Node<Mesh>& node, collections::scene_graph::node_ref parent){
      return scene.createNode( (const Mesh*)&node.data() , parent);
    },collections::scene_graph::Scene<Mesh*>::Root_Ref);
    return scene;
  }

  const auto& meshes() const{
    return m_meshes.nodes();
  }
 
  ~SceneAsset(){

  }
private:
  void handleNode(aiNode* node, const aiScene* scene, collections::scene_graph::node_ref parent){
    //std::cout << "Handling Node meshes: " << node->mNumMeshes << "children " << node->mNumChildren << std::endl;
    auto currentNode = m_meshes.createNode(parent);

    for(unsigned int i = 0; i < node->mNumMeshes; i++){
      aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];  
      Mesh m;
      std::vector<Face<glm::vec4>> vertices;
      vertices.reserve(mesh->mNumFaces*3);
      for(uint32_t i = 0; i < mesh->mNumFaces; i++){
        aiFace face = mesh->mFaces[i];
        Face<glm::vec4> f;
        for(uint32_t j = 0; j < 3; j++){
          f.face[j].x = mesh->mVertices[face.mIndices[j]].x;
          f.face[j].y = mesh->mVertices[face.mIndices[j]].y;
          f.face[j].z = mesh->mVertices[face.mIndices[j]].z;
          f.face[j].w = 1;
        } 
        vertices.push_back(f);
      }
      m.addLayer<Mesh::vertex>(std::move(vertices));
      auto meshNode = m_meshes.createNode(std::move(m),currentNode);
    }

    for(int i = 0; i < node->mNumChildren; i++){
      handleNode(node->mChildren[i],scene,currentNode);
    }
  }
private:
  collections::scene_graph::Scene<Mesh> m_meshes;
};


}
}
