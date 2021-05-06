#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <functional>
#include <vector>
#include <cgeom/transform.h>
#include <collections/sbo_vtable.h>
#include <collections/observable.h>
#include <optional>
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
  class TransformWrapper{
  public:
    TransformWrapper(cgeom::transform::Transform* transform){
      m_transform = transform;
    }

    bool operator()() const {
      return (m_transform != nullptr);
    }

    cgeom::transform::Transform* operator->(){
      assert(m_transform != nullptr);
      return m_transform;
    }

    const cgeom::transform::Transform* operator->() const {
      assert(m_transform != nullptr);
      return m_transform;
    }

  private:
    cgeom::transform::Transform* m_transform = nullptr;  
  };

  virtual TransformWrapper transform() const {return {nullptr}; };
  virtual TransformWrapper transform(const std::string& name) const { return {nullptr}; } ;
  virtual uint32_t uuid() const {return -1;}
  virtual const std::string& uri() const {return "";}
  virtual ~ISceneItemFunctions(){};
};


struct ICameraFunctions{
  virtual cgeom::transform::Transform& transform() = 0;
  virtual ~ICameraFunctions(){} 
};


class IRenderImpl;
class UiManager;


namespace window{
  enum event{
    Quit = 0
  };
  typedef std::function<void(uint32_t)> InputHandler; 
  typedef std::function<void()> UiTab;
  typedef std::function<void()> UiWindow;
  typedef std::function<void()> RenderFunction;
  typedef std::function<void(event e)> EventHandler;
  class IWindow;
}

typedef collections::sbo_vtable::Vtable<ISceneItemFunctions,64> SceneItem;
typedef collections::sbo_vtable::Vtable<ICameraFunctions,64> Camera;

class RenderContext : public collections::Observable<RenderContext, SceneItem> , collections::Observable<RenderContext, Camera>{
  friend collections::Observable<RenderContext,SceneItem>;
  friend collections::Observable<RenderContext,Camera>;
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

  void addInputHandler(const window::InputHandler& listener);
  void addEventHandler(const window::EventHandler& handler);
  void addUiTab(const std::string& name, const window::UiTab& tab);
  void addUiWindow(const std::string& name, const window::UiWindow& window);
  
  template<typename T, typename F>
  void addHandler(const F&  l ){
    collections::Observable<RenderContext,T>::addUpdateHandler(l);
  }

  template<typename T> 
  const auto& items() const {
    return collections::Observable<RenderContext,T>::_items();
  }

  void render();
  window::IWindow* window();
  asset::Manager& assetManager(){return m_assetManager;}

  ~RenderContext();

protected:
  void onItemAdded(const SceneItem& item);
  void onItemAdded(const Camera& camera);

private:
  std::unique_ptr<IRenderImpl> m_impl;
  std::unique_ptr<window::IWindow> m_window;
  std::unique_ptr<UiManager> m_uiManager;
  
  asset::Manager  m_assetManager;
  uint32_t m_assetHandle = 0;
};


}
