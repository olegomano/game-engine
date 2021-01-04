#include "asset.h"


std::ostream& render::asset::operator<<(std::ostream& out, const render::asset::Mesh& a){
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
  return out;
}


std::ostream& render::asset::operator<<(std::ostream& out, const render::asset::SceneAsset& a){
  for(const auto& mesh : a.meshes()){
    out << *mesh << std::endl;
  }
  return out;
}


