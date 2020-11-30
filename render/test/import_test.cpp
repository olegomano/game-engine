#include "gtest/gtest.h"
#include <asset_import/asset.h>
#include <ncurses_impl/ray_tracer.h>
#include <glm/gtx/string_cast.hpp>

TEST(asset_import, import_collada){
  render::asset::SceneAsset asset("../../assets/plane.dae");
  for(const auto& mesh : asset.meshes()){
    //for(auto face = mesh.begin<render::asset::Mesh::vertex>(); face != mesh.end<render::asset::Mesh::vertex>(); ++face){
    for(auto face : mesh.faces<render::asset::Mesh::vertex>()){
      std::cout << "Face: ";
      for(const auto& vertex : face.face){
        std::cout << glm::to_string(vertex);
      }
      std::cout << std::endl;
    }
  } 
}



TEST(asset_import,ray_trace){
  render::asset::SceneAsset asset("../../assets/plane.dae");
  render::ncurses::raytrace::RayTracer tracer(16,16);
  tracer.render(asset.meshes());
  
  std::cout << std::endl;
  int count = 0;
  for(uint32_t pixel : tracer.buffer()){
    ++count;
    std::cout << pixel << " ";
    if(count % tracer.width() == 0 && count != 0){
      std::cout << std::endl;
    }
  }
}

