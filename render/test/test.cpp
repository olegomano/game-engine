#include "gtest/gtest.h"
#include <render/render.h>

TEST(gl_render, create){
  render::RenderContext c;
  c.create(render::RenderContext::GL_3);
}

TEST(asset_import, load){
  render::RenderContext c;
  c.create(render::RenderContext::GL_3);
  c.createAsset("../../assets/cube.dae");  
}


