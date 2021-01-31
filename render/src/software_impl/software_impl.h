#pragma once
#include <memory.h>
#include <render/render.h>
#include <render_impl.h>
#include <window_impl.h>
#include <asset_import/asset.h>
#include <collections/ring_buffer.h>
#include <shaders/flat_color.h>
#include <gl_utils/vbo.h>

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
  SoftwareRenderer();
  void render();
  IAsset addAsset(asset::SceneAsset& asset);
private:
  std::vector<render::asset::SceneAsset*> m_assets;
  std::vector<cgeom::transform::Transform> m_transforms; 
  collections::ring_buffer::RingBuffer<collections::ring_buffer::ArrayWrapper<float,128>> m_frameRate;

  render::shader::flat_color::flat_color m_colorShader;
  render::gl::VBO m_triangleVBO; 
};

}
}
