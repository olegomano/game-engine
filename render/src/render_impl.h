#pragma once
#include <render/render.h>
#include <asset_import/asset.h>
namespace render{

class IRenderImpl{
public:
  typedef uint32_t r_handle;
  virtual void render() = 0;
  virtual r_handle addAsset(const asset::SceneAsset& asset) = 0;

  /**
  virtual void addCamera() = 0;
  virtual void addDrawable() = 0;
  **/

  virtual ~IRenderImpl(){}
};
}
