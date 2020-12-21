#include "asset.h"


std::ostream& render::asset::operator<<(std::ostream& out, const render::asset::Mesh& a){
  for(int i = render::asset::Mesh::LayerType::START + 1; i < render::asset::Mesh::LayerType::END;i++ ){
    if(a.hasLayer( (render::asset::Mesh::LayerType) i)){
      out << i << ": [";
      a.layer((render::asset::Mesh::LayerType)i,[&](const auto& layer){
        for(const auto& face : layer){ 
          out << "{" << glm::to_string(face[0]) << "," << glm::to_string(face[1]) << "," << glm::to_string(face[2]) << "}";
        }
      });
      out << "]" << std::endl;
    }
  }
  return out;
}


std::ostream& render::asset::operator<<(std::ostream& out, const render::asset::SceneAsset& a){
  for(const auto& mesh : a.meshes()){
    out << *mesh << std::endl;
  }
  return out;
}


