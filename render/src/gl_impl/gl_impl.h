#pragma once
#include <render/render.h>


namespace render{
namespace gl{

class GLRender : public IRenderImpl{
public:
  r_handle addAsset(const render::asset::SceneAsset& asset){ return -1;};
  void render(){}
};

}
}
