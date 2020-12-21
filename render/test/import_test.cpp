#include "gtest/gtest.h"
#include <asset_import/asset.h>
#include <ncurses_impl/ray_tracer.h>
#include <glm/gtx/string_cast.hpp>

TEST(asset_import, import_collada){
  render::asset::SceneAsset asset("../../assets/cube.dae");
  std::cout << asset << std::endl;
}

TEST(asset_import, scene_graph_append){
  render::asset::SceneAsset cube("../../assets/cube.dae");
  render::asset::SceneAsset plane("../../assets/plane.dae");

  collections::scene_graph::Scene<const render::asset::Mesh*> scene;
  collections::scene_graph::Scene<const render::asset::Mesh*> cubeScene = cube.scene();
  collections::scene_graph::Scene<const render::asset::Mesh*> planeScene = plane.scene();

  scene.append(cubeScene);
  scene.append(cubeScene);
  scene.append(planeScene);

  scene.update();
  for(const auto& meshTuple : scene.globals()){
    const render::asset::Mesh* m = scene.get_data(meshTuple);
    std::cout << *m << std::endl;
  }
}

TEST(asset_import, scene_graph_asset){
  render::asset::SceneAsset asset("../../assets/plane.dae");
  auto scene = asset.scene();
  scene.update();
  for(auto& globalPosition : scene.globals()){
    const render::asset::Mesh* m = std::get<const render::asset::Mesh* const>(globalPosition);
    std::cout << *m << std::endl;
  }
}

TEST(asset_import,ray_trace){
  render::asset::SceneAsset asset("../../assets/plane.dae");
  auto scene = asset.scene();
  scene.update();
  render::ncurses::raytrace::RayTracer tracer(16,16);
  tracer.render(scene.globals());
  for(int y = 0; y < tracer.height(); y++){
    for(int x = 0; x < tracer.width(); x++){
      std::cout << tracer.buffer()[y * tracer.width() + x] << " ";
    }
    std::cout << std::endl;
  } 
}

TEST(asset_import, scene_graph_print){


}
