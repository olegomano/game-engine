#include "gtest/gtest.h"
#include <asset_import/asset.h>
#include <glm/gtx/string_cast.hpp>

TEST(asset_import, import_collada){
  render::asset::SceneAsset asset("../../assets/cube.dae");
  for(const auto& mesh : asset.meshes()){
    for(auto face = mesh.begin<render::asset::Mesh::vertex>(); face != mesh.end<render::asset::Mesh::vertex>(); ++face){
      std::cout << "Face: ";
      for(const auto& vertex : face->face){
        std::cout << glm::to_string(vertex);
      }
      std::cout << std::endl;
    }
  } 
}


