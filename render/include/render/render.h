#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <functional>
#include <vector>
#include "import.h"


namespace render{
namespace primitive{
enum Value{
  Triangle,
  Cube,
  Rect
};
}


class IAssetFunctions{
public:
  virtual glm::mat4& transform() = 0;
  virtual const glm::mat4& transform() const = 0;
  virtual ~IAssetFunctions(){};
};

/**
 *  Represents an asset inside the rendering pipeline
 *  that has a position and attributes
 *  
 */
class IAsset{
public:
  template<typename T>
  void create(T&& obj){
    static_assert(sizeof(T) < 1024,"Object too large");
    m_null = false;
    T* lhs = (T*)m_impl;
    new(lhs) T(obj);
  }

  glm::mat4& transform(){
    IAssetFunctions* f = (IAssetFunctions*)m_impl;
    return f->transform();
  }

  const glm::mat4& transform() const{
    IAssetFunctions* f = (IAssetFunctions*)m_impl;
    return f->transform(); 
  }

  bool operator()(){
    return !m_null;
  }

  ~IAsset(){
    if(!m_null){
      IAssetFunctions* ptr = (IAssetFunctions*)(m_impl);
      ptr->~IAssetFunctions();
    }
  }
private:
  uint8_t m_impl[1024];
  bool    m_null = true;
};

class IInputManager{
public:
  typedef std::function<void(uint32_t)> InputHandler; 
  virtual void pollInput() = 0;
  void addInputListener(const InputHandler& handler){
    m_handlers.push_back(handler);
  }
protected:
  std::vector<InputHandler> m_handlers;
};

class IRenderImpl;
class RenderContext{
public:

  enum Type{
    TEXT,
    TEXT_RAY,
    GL_3
  };
  RenderContext();
  void create(Type t);
  void createPrimitive(primitive::Value v);
  IAsset createAsset(const std::string& path);
  

  void render();
  void addInputListener(const IInputManager::InputHandler& handler);
  ~RenderContext();
private:
  std::unique_ptr<IRenderImpl> m_impl;
  std::unique_ptr<IInputManager> m_input;
  asset::Manager m_assetManager;

  uint32_t m_assetHandle = 0;
};


}
