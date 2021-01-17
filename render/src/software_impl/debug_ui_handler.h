#pragma once
#include<render/render.h>
#include <render_impl.h>
#include <window_impl.h>
#include <asset_import/asset.h>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include "software_impl.h"

namespace render{
namespace software{ 

class SoftwareRenderer;
class DebugUIHandler{
public:
  DebugUIHandler(const SoftwareRenderer* owner);
  void drawUI();
  ~DebugUIHandler(){}
private:
  const SoftwareRenderer* m_owner;
};

}
}
