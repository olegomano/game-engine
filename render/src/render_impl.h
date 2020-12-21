#pragma once
#include <render/render.h>
#include <asset_import/asset.h>
namespace render{

class IRenderImpl{
public:
  virtual void render() = 0;
  virtual IAsset addAsset(asset::SceneAsset& asset) = 0;
  virtual ~IRenderImpl(){}
};
}
