#include "asset.h"


std::ostream& render::asset::operator<<(std::ostream& out, const render::asset::Mesh& a){
  out << "Name: " << a.name() << std::endl;
  render::asset::Mesh::VertexLayer::for_each([&](auto layerName){
      if(a.hasLayer(layerName)){
      out << render::asset::Mesh::VertexLayer::layer_name(layerName) << ": [";
      a.layer(layerName,[&](const auto& layer){
        for(const auto& face : layer){ 
          out << "{" << glm::to_string(face[0]) << "," << glm::to_string(face[1]) << "," << glm::to_string(face[2]) << "}";
        }
      });
      out << "]" << std::endl;
    }
  });


  out << "Textures: [";
  render::asset::Mesh::TextureLayer::for_each([&](auto textureName){
    if(a.hasTexture(textureName)){
      out << render::asset::Mesh::TextureLayer::layer_name(textureName) << " ";
    }
  });
  out << "]" << std::endl;
  return out;
}


std::ostream& render::asset::operator<<(std::ostream& out, const render::asset::SceneAsset& a){
  int meshIndex = 0;
  for(const auto& mesh : a.meshes()){
    out << "Mesh Index: " << meshIndex++ << std::endl;
    out << mesh << std::endl;
  }
  return out;
}


