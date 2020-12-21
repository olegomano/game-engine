#pragma once
#include <render/render.h>
#include <render_impl.h>

namespace render{
namespace gl{

class GLRender : public IRenderImpl{
public:
  ::render::IAsset addAsset(render::asset::SceneAsset& asset){return {};};
  void render(){}
};

}
}
