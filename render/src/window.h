#pragma once
#include <memory.h>
#include <render/render.h>

namespace render{
namespace window{

struct params{
  uint32_t width;
  uint32_t height;
  std::string name;
};

class IWindow{
public:
  IWindow(const params& params){}
  void addUiTab( const std::string& name, UiTab tab){
    m_uiTabNames.push_back(name);
    m_uiTabs.push_back(tab);
  }
  
  void addUiWindow( const std::string& name, UiWindow window){
    m_uiWindowNames.push_back(name);
    m_uiWindows.push_back(window);
  }

  void addInputHandler(const InputHandler& h){
    m_inputHanlers.push_back(h);
  }

  void addEventHandler(const EventHandler& h){
    m_eventHandlers.push_back(h);
  }

  void addRenderer( RenderFunction r){m_renderers.push_back(r);}
  virtual void render() = 0;
  ~IWindow(){}
  
  virtual void drawUI() = 0;
  virtual void pollInput() = 0;

protected:
  std::vector<UiTab> m_uiTabs;
  std::vector<std::string> m_uiTabNames;

  std::vector<UiWindow> m_uiWindows;
  std::vector<std::string> m_uiWindowNames;
  
  std::vector<RenderFunction> m_renderers;
  
  std::vector<InputHandler> m_inputHanlers;
  std::vector<EventHandler> m_eventHandlers;

};


}}

