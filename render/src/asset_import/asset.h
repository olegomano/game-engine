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
#include <collections/debug_print.h>
#include <collections/scene_graph.h>
#include <collections/stable_vector.h>
#include <cgeom/transform.h>
#include "texture.h"
#include "mesh.h"

namespace render{
namespace asset{

class SceneAsset{
public:    
  friend std::ostream& operator<<(std::ostream& os, const SceneAsset& sa);
  
  SceneAsset(const std::string& path) : m_path(path){
    m_woringDir = path.substr(0,path.find_last_of("/"));

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);       
    if(scene == nullptr){
      debug::print::print_error("Failed to open file",path);
      return;
    }
    handleNode(scene->mRootNode,scene,collections::scene_graph::Scene<Mesh>::Root_Ref);
  }

  collections::scene_graph::Scene<const Mesh*> scene(){
    return m_scene;
  }

  const Mesh* findMesh(const std::string& name) const {
    for(const collections::scene_graph::Node<const Mesh*>& meshNode : m_scene.nodes() ){
      if((*meshNode)->name() == name){
        return (const Mesh*)((*meshNode));
      }
    } 
    return nullptr;
  }

  const auto& meshes() const{
    return m_meshes;
  }

  const std::string& path() const {
    return m_path;
  }
 
  ~SceneAsset(){

  }
private:
  void handleNode(aiNode* node, const aiScene* scene, collections::scene_graph::node_ref parent){
    //std::cout << "Handling Node meshes: " << node->mNumMeshes << "children " << node->mNumChildren << std::endl;
    auto currentNode = m_scene.createNode(parent);
    aiMatrix4x4 nodeTransform = node->mTransformation;
    glm::mat4 transform;

    for(int x = 0; x < 4; x++){
      for(int y = 0; y < 4; y++){
        transform[x][y] = nodeTransform[x][y];
      }
    }
    m_scene[currentNode].transform().transform() = transform;  
    
    for(unsigned int i = 0; i < node->mNumMeshes; i++){
      aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];  
      Mesh m = handleMesh(mesh,scene);
      m_meshes.push_back(std::move(m));
      auto meshNode = m_scene.createNode( &(m_meshes[m_meshes.size() - 1]),currentNode);
    }

    for(int i = 0; i < node->mNumChildren; i++){
      handleNode(node->mChildren[i],scene,currentNode);
    }
  }

  Mesh handleMesh(aiMesh* mesh, const aiScene* scene){
    Mesh m;
    m.setName(mesh->mName.data); 
    if(mesh->HasFaces()){
      std::vector<Face<glm::vec4>> vertices;
      std::vector<Face<glm::vec4>> normals;
      std::vector<Face<glm::vec2>> uv;
      
      vertices.reserve(mesh->mNumFaces*3); 
      normals.reserve(mesh->mNumFaces*3);
      uv.reserve(mesh->mNumFaces*3);

      for(uint32_t i = 0; i < mesh->mNumFaces; i++){
        aiFace face = mesh->mFaces[i];
        Face<glm::vec4> vertFace;
        Face<glm::vec4> normalFace;
        Face<glm::vec2> uvFace;

        for(uint32_t j = 0; j < 3; j++){  
          if(mesh->HasPositions()){
            vertFace.face[j].x = mesh->mVertices[face.mIndices[j]].x;
            vertFace.face[j].y = mesh->mVertices[face.mIndices[j]].y;
            vertFace.face[j].z = mesh->mVertices[face.mIndices[j]].z;
            vertFace.face[j].w = 1;
          }
          if(mesh->HasNormals()){
            normalFace.face[j].x = mesh->mNormals[face.mIndices[j]].x;
            normalFace.face[j].y = mesh->mNormals[face.mIndices[j]].y;
            normalFace.face[j].z = mesh->mNormals[face.mIndices[j]].z;
            normalFace.face[j].w = 0;
          }
          if(mesh->HasTextureCoords(0)){
            uvFace.face[j].x = mesh->mTextureCoords[0][face.mIndices[j]].x;
            uvFace.face[j].y = mesh->mTextureCoords[0][face.mIndices[j]].y;
          }
        } 
        vertices.push_back(std::move(vertFace));
        normals.push_back(std::move(normalFace));
        uv.push_back(std::move(uvFace));
      }


      if(mesh->HasPositions()){
        m.addLayer<Mesh::VertexLayer::vertex>(std::move(vertices));
      }
      if(mesh->HasNormals()){
        m.addLayer<Mesh::VertexLayer::normal>(std::move(normals));
      }
      if(mesh->HasTextureCoords(0)){
        m.addLayer<Mesh::VertexLayer::uv_1>(std::move(uv));
      }
    }
    //handle textures
    const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    std::string diffusePath;
    if(getTexture(aiTextureType_DIFFUSE,material,diffusePath)){
      diffusePath = m_woringDir + "/" + diffusePath;
      if(!m_textures.count(diffusePath)){
        Texture txt = Texture(diffusePath);
        debug::print::print_debug("Loaded texture",diffusePath," width: ",txt.width(), " height: ", txt.height() );
        m_textures[diffusePath] = std::move(txt);
      }
      m.addTexture<Mesh::TextureLayer::diffuse>(&(m_textures[diffusePath]));      
    } 
    return m;
  }

  bool getTexture(aiTextureType type, const aiMaterial* material, std::string& path){
    if(!material->GetTextureCount(type)){
      return false;
    }
    aiString textureName;
    if( material->Get(AI_MATKEY_TEXTURE(type, 0), textureName) != AI_SUCCESS ){
      return false;
    }
    path = textureName.data;
    return true;
  }

private:
  collections::scene_graph::Scene<const Mesh*> m_scene;
  collections::stable_vector::StableVector<Mesh> m_meshes;
  std::unordered_map<std::string,Texture> m_textures;

  const std::string m_path;
  std::string m_woringDir;
};


}
}
