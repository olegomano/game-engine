#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <functional>
#include <vector>
#include <cgeom/transform.h>
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
  virtual cgeom::transform::Transform& transform() = 0;
  virtual const cgeom::transform::Transform& transform() const = 0;
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
    static_assert(sizeof(T) < 32,"Object too large");
    m_null = false;
    T* lhs = (T*)m_impl;
    new(lhs) T(obj);
  }

  cgeom::transform::Transform& transform(){
    IAssetFunctions* f = (IAssetFunctions*)m_impl;
    return f->transform();
  }

  const cgeom::transform::Transform& transform() const{
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
  uint8_t m_impl[32];
  bool    m_null = true;
};

class IInputManager{
public:
  enum event{
    Quit
  };

  typedef std::function<void(uint32_t)> InputHandler; 
  typedef std::function<void(event e)> EventHandler;

  virtual void pollInput() = 0;
  void addInputListener(const InputHandler& handler){
    m_handlers.push_back(handler);
  }

  void addEventListener(const EventHandler& handler){
    m_eventHandler.push_back(handler);
  }
protected:
  void sendKeyEvent(uint32_t event){
    for(auto& handler : m_handlers){
      handler(event);
    }
  }

  void sendEvent(event e){
    for(auto& handler : m_eventHandler){
      handler(e);
    }
  }

  std::vector<InputHandler> m_handlers;
  std::vector<EventHandler> m_eventHandler;
};

class IRenderImpl;
namespace window{
  class IWindow;
}


class RenderContext{
public:
  enum Type{
    TEXT,
    TEXT_RAY,
    SOFTWARE,
    GL_2,
    GL_3
  };
  RenderContext();
  void create(Type t);
  void createPrimitive(primitive::Value v);
  IAsset createAsset(const std::string& path);

  void render();
  window::IWindow* window();
  IInputManager* input();
  ~RenderContext();
private:

  void drawAssetInspectorUI();
  std::unique_ptr<IRenderImpl> m_impl;
  std::unique_ptr<IInputManager> m_input;
  std::unique_ptr<window::IWindow> m_window;
  asset::Manager m_assetManager;

  uint32_t m_assetHandle = 0;
};


}
