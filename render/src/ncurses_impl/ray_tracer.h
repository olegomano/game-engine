#pragma once
#include <memory.h>
#include <cstdint>
#include <asset_import/asset.h>
#include <cgeom/intersect.h>

namespace render{
namespace ncurses{
namespace raytrace{


class RayTracer{
public:
  RayTracer(uint32_t w, uint32_t h) : m_buffer(w*h), m_width(w), m_height(h){
    m_camera = glm::mat4(1.0);
  }
 
  glm::mat4& camera(){
    return m_camera;
  }

  const std::vector<uint32_t>& buffer(){
    return m_buffer;
  }

  uint32_t width() const {
    return m_width;
  }

  uint32_t height() const {
    return m_height;
  }

  template<typename _T_MeshList>
  void render(const _T_MeshList& mesh){
    memset(&m_buffer[0], 0xffffffff,m_buffer.size() * 4);

    float pixelWidth = m_sensorWidth  / (float)m_width;
    float pixelHeight = m_sensorHeight / (float)m_height;
    const glm::vec3 cameraOrigin = {0,0,-1 * m_focalLength};

    for(int y = 0; y < m_height; y++){
      for(int x = 0; x < m_width; x++){
        float px = x*pixelWidth - m_sensorWidth/2;
        float py = m_sensorHeight/2 - y*pixelHeight;
 
        uint32_t color = castRay(
            cameraOrigin, 
            glm::normalize(glm::vec3(px,py,0) - cameraOrigin),
            mesh); 
        m_buffer[y*m_width + x] = color;
      }
    }
  }

private:
  template<typename _T_MeshList>
  uint32_t castRay(const glm::vec3& origin,const glm::vec3& ray, const _T_MeshList& meshList) const {
    cgeom::intersect::Result result = {{},{},1000};
    bool hasCollision = false;
    
    //std::cout << glm::to_string(ray) << std::endl;
    for(const ::render::asset::Mesh& mesh : meshList){
      for(const auto& face : mesh.faces<::render::asset::Mesh::vertex>()){
        cgeom::intersect::Result r = cgeom::intersect::ray_tris(origin,ray,face);
        if(ray[0] == 0 && ray[1] == 0){
          std::cout <<glm::to_string(origin) << " " << glm::to_string(ray) << std::endl;
          std::cout << r << std::endl;
        }
        if(r.distance < result.distance && r.distance > 0 && r.location.z >= 0){
          result = r;
          hasCollision = true;
        }
      } 
    }
    if(!hasCollision){
     return 0;
    }
    return ceil(result.distance);
  }

 private:
  glm::mat4 m_camera;
  float     m_focalLength = 0.5f;
  float     m_sensorWidth = 1.0f;
  float     m_sensorHeight = 1.0f;
  std::vector<uint32_t> m_buffer;
  const uint32_t m_width;
  const uint32_t m_height;

};


}
} 
}
