#include <render/import.h>
#include "asset.h"

render::asset::SceneAsset* render::asset::Manager::load(const std::string& path){
  render::asset::SceneAsset* asset = new render::asset::SceneAsset(path);
  m_assets[path]=asset;
  return asset;
}


