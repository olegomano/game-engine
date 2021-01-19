#include <imgui.h>
#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl2.h>
#include<asset_import/asset.h>
#include <glm/glm.hpp>
#include <sstream>
#include"debug_ui_handler.h"
#include "ui_helpers.h"

namespace render{
namespace software{


DebugUIHandler::DebugUIHandler(const SoftwareRenderer* owner):m_owner(owner){}
void DebugUIHandler::drawUI(){ 
  if(ImGui::BeginTabItem("Assets")){
    for(const auto& asset : m_owner->m_assets){
      if(ImGui::TreeNode(asset->path().c_str())){
        render::software::ui<std::decay<decltype(asset)>::type>::draw(asset);
        ImGui::TreePop();
      }
    } 
    ImGui::EndTabItem();
  }

  if(ImGui::BeginTabItem("Scene")){
    ImGui::EndTabItem();
  }
}


}
}
