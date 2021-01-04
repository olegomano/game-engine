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
#include <cgeom/transform.h>

namespace render{
namespace asset{

template<typename T>
struct Face{
  std::array<T,3> face;
  
  const T& operator[](uint8_t indx) const {
    assert(indx < 3);
    return face[indx];
  }

  T& operator[](uint8_t indx) {
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

class Texture{
public:
  Texture(const std::string& path){
    
  }

  const std::vector<uint8_t>& pixels() const {
    return m_pixels;
  }

  const uint32_t width() const {
    return m_width;
  }

  const uint32_t height() const {
    return m_height;
  }

private:
  uint32_t m_width  = 0;
  uint32_t m_height = 0;
  std::vector<uint8_t> m_pixels;
};

class Mesh{  
public:    
  friend std::ostream& operator<<(std::ostream& os, const Mesh& m);
  
  struct VertexLayer{
    enum id{
      START = -1,
      vertex,
      uv_1,
      uv_2,
      normal,
      bone,
      END
    };
    
    template<typename T, uint32_t _T_Type>
    struct type{
      static constexpr id _id = (id)_T_Type;
      std::optional<std::vector<Face<T>>> faces;
    };
    
    inline static std::string layer_name(id layer){
      switch(layer){
      case vertex:
        return "vertex";
      case uv_1:
        return "uv_1";
      default:
        return "unknown";
      }
    };

    template<typename T>
    inline static void for_each(T&& f){
      for(uint32_t i = START + 1; i < END; i++){
        f((id)i);
      }
    }
  };

  
  struct TextureLayer{
    enum id{
      START = -1,
      diffuse,
      normal,
      displace,
      END
    };
    
    template<uint32_t _T_Type>
    struct type {
      static constexpr id _id = (id)_T_Type;
      std::optional<Texture> texture;
    };
  }; 

 
  typedef std::tuple<
    VertexLayer::type<glm::vec4,VertexLayer::vertex>,
    VertexLayer::type<glm::vec2,VertexLayer::uv_1>,
    VertexLayer::type<glm::vec2,VertexLayer::uv_2>,
    VertexLayer::type<glm::vec4,VertexLayer::normal>,
    VertexLayer::type<glm::vec4,VertexLayer::bone>
  >VertexGroup;
  
  template<typename VertexLayer::id T, typename Vector>
  void addLayer(Vector&& vector){
    std::get<T>(m_layers).faces = std::move(vector);
  }  
  
  bool hasLayer(const typename VertexLayer::id& t) const {
    return std::apply([&](auto&... layer){    
      return ( ( (layer._id == t) && layer.faces.has_value()) || ... );    
    },m_layers);
  }

  template<typename VertexLayer::id T>
  bool hasLayer() const {
    return std::get<T>(m_layers).faces.has_value();
  }
  
  template<typename VertexLayer::id T>
  const auto& layer() const {
    assert(hasLayer<T>());
    return std::get<T>(m_layers).faces.value();
  }

  template<typename Callback>
  void layer(const typename VertexLayer::id& t, Callback&& callback) const {
    std::apply([&](auto&... layer){
      std::make_tuple( dynamicApplyLayer(t,layer,callback) ... );
    },m_layers);
  }

  const std::string& name() const {
    return m_name;
  }

private:
  
  template<typename Callback, typename Layer>
  bool dynamicApplyLayer(const typename VertexLayer::id& type, const Layer& layer, Callback&& callback) const {
    if(type == layer._id){
      callback(layer.faces.value());
      return true;
    }
    return false;
  }
  
  VertexGroup m_layers;
  std::string m_name = "";
  
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
      m.addLayer<Mesh::VertexLayer::vertex>(std::move(vertices));
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
