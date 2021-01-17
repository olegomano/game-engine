#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include <variant>
#include <optional>
#include <tuple>
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <collections/debug_print.h>
#include <collections/scene_graph.h>
#include <cgeom/transform.h>
#include "texture.h"
namespace render{
namespace asset{

template<typename T>
struct Face{
  std::array<T,3> face;
  
  const T& operator[](uint8_t indx) const {
    assert(indx < 3);
    return face[indx];
  }

  T& operator[](uint8_t indx) {
    assert(indx < 3);
    return face[indx];
  }
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const Face<T>& face){
  os << "[";
  for(const auto& v : face.face){
    os << glm::to_string(v) << ", ";
  }
  os << "]";
  return os;
}

class Mesh{  
public:    
  friend std::ostream& operator<<(std::ostream& os, const Mesh& m);
  
  struct VertexLayer{
    enum id{
      START = -1,
      vertex,
      uv_1,
      uv_2,
      normal,
      bone,
      END
    };
    
    template<typename T, uint32_t _T_Type>
    struct type{
      static constexpr id _id = (id)_T_Type;
      std::optional<std::vector<Face<T>>> faces;
    };
    
    inline static std::string layer_name(id layer){
      switch(layer){
      case vertex:
        return "vertex";
      case normal:
        return "normal";
      case uv_1:
        return "uv_1";
      default:
        return "unknown";
      }
    };

    template<typename T>
    inline static void for_each(T&& f){
      for(uint32_t i = START + 1; i < END; i++){
        f((id)i);
      }
    }
  };

  
  struct TextureLayer{
    enum id{
      START = -1,
      diffuse,
      normal,
      displace,
      END
    };
    
    inline static std::string layer_name(id layer){
      switch(layer){
      case diffuse:
        return "diffuse";
      case normal:
        return "normal";
      case displace:
        return "displace";
      default:
        return "unknown";
      }
    };

    template<typename T>
    inline static void for_each(T&& f){
      for(uint32_t i = START + 1; i < END; i++){
        f((id)i);
      }
    }

    template<uint32_t _T_Type>
    struct type {
      static constexpr id _id = (id)_T_Type;
      std::optional<Texture*> texture;
    };
  }; 

 
  typedef std::tuple<
    VertexLayer::type<glm::vec4,VertexLayer::vertex>,
    VertexLayer::type<glm::vec2,VertexLayer::uv_1>,
    VertexLayer::type<glm::vec2,VertexLayer::uv_2>,
    VertexLayer::type<glm::vec4,VertexLayer::normal>,
    VertexLayer::type<glm::vec4,VertexLayer::bone>
  >VertexGroup;


  typedef std::tuple<
    TextureLayer::type<TextureLayer::diffuse>,
    TextureLayer::type<TextureLayer::normal>,
    TextureLayer::type<TextureLayer::displace>
  > TextureGroup;
  
  
  void setName(const std::string& name){
    m_name = name;
  }

  template<TextureLayer::id T>
  void addTexture(Texture* txt){
    std::get<T>(m_textures).texture = txt;
  }


  template<TextureLayer::id T>
  bool hasTexture() const {
    return std::get<TextureLayer::type<T>>(m_textures).texture.has_value();
  }

  bool hasTexture(const typename TextureLayer::id& t) const {
   return std::apply([&](auto&... layer){    
      return ( ( (layer._id == t) && layer.texture.has_value()) || ... );    
    },m_textures);  
  }

  const Texture* const texture(TextureLayer::id id) const {
    Texture* result = nullptr; 
    auto checker = [&](const auto& txt){
      if(txt._id == id){
        result = txt.texture.value(); 
      }
      return true;
    };

    std::apply([&](auto&... layer) mutable {
      return ( ( checker(layer) ) || ... );      
    },m_textures);
    return (const Texture*) result;
  }

  template<TextureLayer::id T>
  const Texture* const texture() const {
    assert(hasTexture<T>());
    return (const Texture* const) (std::get<TextureLayer::type<T>>(m_textures).texture.value());
  }

  template<typename VertexLayer::id T, typename Vector>
  void addLayer(Vector&& vector){
    if(m_faceCount == 0){
      m_faceCount = vector.size();
    }
    assert(m_faceCount == vector.size());
    std::get<T>(m_layers).faces = std::move(vector);
  }  
  
  bool hasLayer(const typename VertexLayer::id& t) const {
    return std::apply([&](auto&... layer){    
      return ( ( (layer._id == t) && layer.faces.has_value()) || ... );    
    },m_layers);
  }

  template<typename VertexLayer::id T>
  bool hasLayer() const {
    return std::get<T>(m_layers).faces.has_value();
  }
  
  /**
   * returns a vector<Face<?>>
   */
  template<typename VertexLayer::id T>
  const auto& layer() const {
    assert(hasLayer<T>());
    return std::get<T>(m_layers).faces.value();
  }

  template<typename Callback>
  void layer(const typename VertexLayer::id& t, Callback&& callback) const {
    std::apply([&](auto&... layer){
      std::make_tuple( dynamicApplyLayer(t,layer,callback) ... );
    },m_layers);
  }

  const std::string& name() const {
    return m_name;
  }

  const uint32_t faceCount() const {
    return m_faceCount;
  }

private:
  
  template<typename Callback, typename Layer>
  bool dynamicApplyLayer(const typename VertexLayer::id& type, const Layer& layer, Callback&& callback) const {
    if(type == layer._id){
      callback(layer.faces.value());
      return true;
    }
    return false;
  }
  
  VertexGroup m_layers;
  TextureGroup m_textures;
  std::string m_name = "";
  uint32_t m_faceCount = 0;
};



}
}
