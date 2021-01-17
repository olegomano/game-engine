#pragma once
#include <memory.h>
#include <render/render.h>
#include <render_impl.h>
#include <window_impl.h>
#include <asset_import/asset.h>
#include "debug_ui_handler.h"

namespace render{
namespace software{ 

class SoftwareRenderer;
class DebugUIHandler;

class Asset : public IAssetFunctions{
public:
  Asset(uint32_t index, SoftwareRenderer* owner);
  cgeom::transform::Transform& transform() override;
  const cgeom::transform::Transform& transform() const override;
private:
  uint32_t m_index;
  SoftwareRenderer* m_owner = nullptr;
};

class SoftwareRenderer : public IRenderImpl{ 
public:
  friend Asset;
  friend DebugUIHandler;
  SoftwareRenderer(const ::render::window::params& params);
  void render() override;
  void addUiHandler(const ::render::RenderContext::UiDrawHandler& handler) override;
  void onRawInputEvent(SDL_Event& event); 
  IAsset addAsset(asset::SceneAsset& asset);
private:
  render::window::GLES2Window m_window;
  std::vector<render::asset::SceneAsset*> m_assets;
  std::vector<cgeom::transform::Transform> m_transforms;
  std::vector<::render::RenderContext::UiDrawHandler> m_uiHandlers;
  std::unique_ptr<DebugUIHandler> m_debugUI;
};

}

;
}
