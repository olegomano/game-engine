#pragma once
#include <render/render.h>
#include <render_impl.h>
#include <memory>
#include <curses.h>
#include <vector>

namespace render{
namespace ncurses{

class InputManager : public IInputManager{
public:
  InputManager();
  void handleKeyboard(uint32_t key);
  void pollInput();
};

class NCursesRender : public IRenderImpl{
public:
  NCursesRender();
  void render() override;
  r_handle addAsset(const ::render::asset::SceneAsset& asset) override;
  ~NCursesRender();
private:
  uint32_t m_count = 0;
  std::vector<const ::render::asset::SceneAsset*> m_assets;
};

}
}
