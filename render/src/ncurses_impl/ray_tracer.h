#pragma once
#include <memory.h>
#include <cstdint>
#include <asset_import/asset.h>
#include <cgeom/intersect.h>
#include <collections/debug_print.h>
#include <asset_import/texture.h>

namespace render{
namespace ncurses{
namespace raytrace{

struct Result{
  float distance;
  uint32_t color;
  glm::vec4 normal;
  glm::vec4 position;
  glm::vec2 uv;
};

class RayTracer{
public:
  RayTracer(uint32_t w, uint32_t h) : m_buffer(w*h), m_width(w), m_height(h){
    m_camera = glm::mat4(1.0);
  }
 
  glm::mat4& camera(){
    return m_camera;
  }

  const std::vector<Result>& buffer() const{
    return m_buffer;
  }

  uint32_t width() const {
    return m_width;
  }

  uint32_t height() const {
    return m_height;
  }
  
  /**
   *  _T_MeshList is assumed to be an iteratable of a tuple that has a Mesh* and a glm::mat4 
   */
  template<typename _T_MeshList>
  void render(const _T_MeshList& meshList){
    memset(&m_buffer[0], 0xffffffff,m_buffer.size() * 4);
    float pixelWidth = m_sensorWidth  / (float)m_width;
    float pixelHeight = m_sensorHeight / (float)m_height;
    const glm::vec3 cameraOrigin = {0,0,-1 * m_focalLength};

    for(int y = 0; y < m_height; y++){
      for(int x = 0; x < m_width; x++){
        float px = x*pixelWidth - m_sensorWidth/2;
        float py = m_sensorHeight/2 - y*pixelHeight;
 
        Result color = castRay(
            cameraOrigin, 
            glm::normalize(glm::vec3(px,py,0) - cameraOrigin),
            meshList); 
        m_buffer[y*m_width + x] = color;
      }
    }
  }

  template<typename _T_Out>
  void displayBuffer(_T_Out&& out) const {
    for(int y = 0; y < m_height; y++){
      for(int x = 0; x < m_width; x++){
        out << m_buffer[y * m_width + x].distance << " ";
      }
      out << std::endl;
    }
  }

private:
  /**
   * casts a ray against a iteratable of tuple<const render::asset::Mesh* const, glm::mat4>
   * returns {position,uv,color,normal}
   */
  template<typename _T_MeshList>
  Result castRay(const glm::vec3& origin,const glm::vec3& ray, const _T_MeshList& meshList) const {
    Result result;    
    result.distance = 2^32;
    const render::asset::Texture* texture = nullptr;
    bool hasCollision = false;
    
    //std::cout << glm::to_string(ray) << std::endl;
    for(const auto& meshTuple : meshList){
      //TODO: convert this to sfinae so we can handle both ref and pointer
      const render::asset::Mesh* const mesh = std::get<const render::asset::Mesh* const>(meshTuple);
      const glm::mat4& transform = std::get<glm::mat4>(meshTuple);
      
      if(!mesh->hasLayer<::render::asset::Mesh::VertexLayer::vertex>()){
        continue;
      }
      //debug::print::print_debug("Casting Ray Against: ", mesh->layer<::render::asset::Mesh::vertex>().size(), "");
      for(const auto& face : mesh->layer<::render::asset::Mesh::VertexLayer::vertex>()){
        render::asset::Face<glm::vec4> globalFace;
        globalFace[0] = transform * face[0];
        globalFace[1] = transform * face[1];
        globalFace[2] = transform * face[2];
        
        /**
         * cgeom::intersect::Result; 
         glm::vec3 location;
         glm::vec3 barycentric;
         float     distance;
         */
        const cgeom::intersect::Result& r = cgeom::intersect::ray_tris(origin,ray,globalFace);       
        if(r.distance < result.distance && r.distance > 0 && r.location.z >= 0){
          result.distance = r.distance;
          result.uv       = r.barycentric;
          result.position = glm::vec4(r.location.x,r.location.y,r.location.z,1); 
          hasCollision = true;
          if(mesh->hasTexture<render::asset::Mesh::TextureLayer::diffuse>()){
            texture = mesh->texture<render::asset::Mesh::TextureLayer::diffuse>();
          }else{
            texture = nullptr;
          } 
        }
      } 
    }
    if(!hasCollision){
      result.distance = 0.0f;
      return result;
    }
    if(texture == nullptr){
      result.color = 0xffffffff;
    }else{
      //result.color = texture->sample(result.uv.x,result.uv.y);
    }
    return result;
  }

 private:
  glm::mat4 m_camera;
  float     m_focalLength = 0.5f;
  float     m_sensorWidth = 1.0f;
  float     m_sensorHeight = 1.0f;
  std::vector<Result> m_buffer;
  const uint32_t m_width;
  const uint32_t m_height;

};


}
} 
}
