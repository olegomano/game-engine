#include "software_impl.h"
#include "debug_ui_handler.h"
#include "ui_helpers.h"
#include <imgui.h>
#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl2.h>
#include <collections/timer.h>

render::software::Asset::Asset(uint32_t index, SoftwareRenderer* owner) : m_index(index),m_owner(owner){
}

cgeom::transform::Transform& render::software::Asset::transform(){
  assert(m_owner != nullptr);
  assert(m_index != -1);
  return m_owner->m_transforms[m_index]; 
}

const cgeom::transform::Transform& render::software::Asset::transform() const {
  assert(m_owner != nullptr);
  assert(m_index != -1);
  return m_owner->m_transforms[m_index]; 
}


//////////////////SoftwareRenderer
render::software::SoftwareRenderer::SoftwareRenderer(const render::window::params& p) :
  m_window(render::window::GLES2Window(p)){
    m_debugUI = std::make_unique<DebugUIHandler>(this);
    addUiHandler(std::bind(&DebugUIHandler::drawUI,m_debugUI.get()));
   
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(m_window.window(), m_window.context());
    ImGui_ImplOpenGL2_Init();
    
    render::shader::flat_color::flat_color colorShader;
    colorShader.compile();
}

static int count = 0;
void render::software::SoftwareRenderer::render(){
  count++;
  m_window.clear();
  debug::timer::ImmidiateTimer drawUI([&](){
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplSDL2_NewFrame(m_window.window());
    ImGui::NewFrame();
  
    ui<std::decay<decltype(m_frameRate)>::type>::draw(m_frameRate);
    ImGui::BeginTabBar("TabBar",0); 
    for(auto& handler : m_uiHandlers){
      handler();
    }
    ImGui::EndTabBar();
    ImGui::Render();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
  });
  m_frameRate.push_back(drawUI.time());
  m_window.swapBuffers();
}

void render::software::SoftwareRenderer::onRawInputEvent(SDL_Event& event){
  //ImGui_ImplSDL2_ProcessEvent(&event);
} 

void render::software::SoftwareRenderer::addUiHandler(const ::render::RenderContext::UiDrawHandler& handler){
  m_uiHandlers.push_back(handler);
}

render::IAsset render::software::SoftwareRenderer::addAsset(::render::asset::SceneAsset& asset){
  m_assets.push_back(&asset);
  m_transforms.push_back(glm::mat4(1.0)); 
  render::IAsset assetWrapper;
  assetWrapper.create<Asset>( Asset( (uint32_t)(m_assets.size() - 1), this) );
  return assetWrapper;
}




