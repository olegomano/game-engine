#pragma once
#include <memory.h>
#include <cstdint>
#include <asset_import/asset.h>

namespace render{
namespace ncurses{
namespace raytrace{


class RayTracer{
public:
  RayTracer(uint32_t w, uint32_t h) : m_buffer(w*h), m_width(w), m_height(h){}
  
  void render(const ::render::asset::Mesh& mesh){
    for(int x = 0; x < m_width; x++){
      for(int y = 0; y < m_height; y++){
        
      }
    }
  }
private:
  uint32_t castRay(uint32_t x, uint32_t y, const ::render::asset::Mesh& m) const {

  }

  glm::vec2 intersect(const glm::vec4& origin, const glm::vec4& ray, const ::render::asset::Face<glm::vec4>& face) const { 
  
  }

private:
  std::vector<uint8_t> m_buffer;
  const uint32_t m_width;
  const uint32_t m_height;

};


}
} 
}
