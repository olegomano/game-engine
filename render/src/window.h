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
  ~IWindow(){}
};

class SDLInputManager : public ::render::IInputManager{
public:
  void pollInput() override;
};

}
}
