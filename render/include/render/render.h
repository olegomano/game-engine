#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <functional>
#include <vector>
#include <cgeom/transform.h>
#include <collections/sbo_vtable.h>
#include "import.h"


namespace render{

namespace primitive{
enum Value{
  Triangle,
  Cube,
  Rect
};
}


class ISceneItemFunctions{
public:
  virtual cgeom::transform::Transform& transform() = 0;
  virtual const cgeom::transform::Transform& transform() const = 0;
  virtual ~ISceneItemFunctions(){};
};

struct ICameraFunctions{
  virtual cgeom::transform::Transform& transform() = 0;
  virtual ~ICameraFunctions(){} 
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

typedef collections::sbo_vtable::Vtable<ISceneItemFunctions,64> SceneItem;
typedef collections::sbo_vtable::Vtable<ISceneItemFunctions,64> Camera;

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
  SceneItem loadAsset(const std::string& path);
  Camera createCamera();

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
