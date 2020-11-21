#pragma once
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

class IRenderImpl;

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
  void createAsset(const std::string& path);
  void render();
  void addInputListener(const IInputManager::InputHandler& handler);
  ~RenderContext();
private:
  std::unique_ptr<IRenderImpl> m_impl;
  std::unique_ptr<IInputManager> m_input;
  asset::Manager m_assetManager;
};


}
