#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include <variant>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

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

struct Layer{
  std::variant<
      std::vector<Face<glm::vec4>>,
      std::vector<Face<glm::vec3>>,
      std::vector<Face<glm::vec2>>
  > data;
};


class Mesh{  
public:    
  friend std::ostream& operator<<(std::ostream& os, const Mesh& m);
  enum LayerType{
    vertex,
    uv_1,
    uv_2,
    normal,
    bone
  };

  
  template<LayerType T,typename V>
  void addLayer(std::vector<Face<V>>& v){
    Layer l;
    l.data = std::move(v);
    m_layers[T] = std::move(l);
  }

  bool hasLayer(LayerType l) const {
    if(m_layers.count(l)){
      return true;
    }
    return false;
  }

  const Layer& layer(LayerType l) const {
    assert(hasLayer(l));
    return m_layers.at(l);
  }

  template<LayerType T>
  auto faces() const {
    assert(hasLayer(T));
    return std::get<T>(layer(T).data);
  }

  template<LayerType T>
  auto begin() const {
    assert(hasLayer(T));
    return std::get<T>( layer(T).data ).cbegin(); 
  }

  template<LayerType T>
  auto end() const{
    assert(hasLayer(T));
    return std::get<T>( layer(T).data ).cend(); 
  }

  const glm::mat4& transform() const {
    return m_transform;
  }

private:
  glm::mat4 m_transform;
  std::unordered_map<LayerType,Layer> m_layers;  
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
    handleNode(scene->mRootNode,scene);
  }

  const auto& meshes() const {
    return m_meshes;
  }
  
  ~SceneAsset(){

  }
private:
  void handleNode(aiNode* node, const aiScene* scene){
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
        } 
        vertices.push_back(f);
      }
      m.addLayer<Mesh::vertex>(vertices);  
      m_meshes.push_back(std::move(m));
    }

    for(unsigned int i = 0; i < node->mNumChildren; i++){
      handleNode(node->mChildren[i], scene); 
    }  
  }
private:
  std::vector<Mesh> m_meshes;
};


}
}
