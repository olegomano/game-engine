#pragma once
#include <memory.h>
#include <render/render.h>

namespace render{
namespace window{

typedef std::function<void()> UiTab;
typedef std::function<void()> UiWindow;
typedef std::function<void()> RenderFunction;

struct params{
  uint32_t width;
  uint32_t height;
  std::string name;
};

class IWindow{
public:
  IWindow(const params& params){}
  void addUiTab( UiTab tab){m_uiTabs.push_back(tab);}
  void addUiWindow( UiWindow window){m_uiWindows.push_back(window);}
  void addRenderer( RenderFunction r){m_renderers.push_back(r);}
  virtual void render() = 0;
  ~IWindow(){}
  
  virtual void drawUI() = 0;

protected:
  std::vector<UiTab> m_uiTabs;
  std::vector<UiWindow> m_uiWindows;
  std::vector<RenderFunction> m_renderers;
};

class SDLInputManager : public ::render::IInputManager {
public:
  void pollInput() override;
};

}
}
