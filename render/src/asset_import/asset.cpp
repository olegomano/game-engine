#include "asset.h"



std::ostream& render::asset::operator<<(std::ostream& out, const render::asset::SceneAsset& a){
  for (const Mesh& m : a.m_meshes){
    out << m << std::endl;
  } 
  return out;
}

std::ostream& render::asset::operator<<(std::ostream& out, const render::asset::Mesh& a){
  for( auto const& [ type,layer ] : a.m_layers ){
    out << type << ": ";
    std::visit([&](const auto& l){
      out << l.size();
    },layer.data);
    out << std::endl;
  }
  return out;
  //std::unordered_map<LayerType,Layer> m_layers;  
}
