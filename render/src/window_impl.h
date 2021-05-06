#pragma once
#include "window.h"
#include <imgui.h>
#include <SDL2/SDL.h>
#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl2.h>
#include <GL/glew.h>
#include <GL/glut.h>

namespace render{
namespace window{

class GLES2Window : public IWindow{
public:
  GLES2Window(const params& params) : IWindow(params){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
 
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    m_window = SDL_CreateWindow(
      params.name.c_str(),
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      params.width,
      params.height,
      window_flags
    );
 

    m_glContext = SDL_GL_CreateContext(m_window);
    SDL_GL_MakeCurrent(m_window, m_glContext);
    SDL_GL_SetSwapInterval(0);

    m_surface   = SDL_GetWindowSurface(m_window);    
    glewInit();

   
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();
    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(m_window, m_glContext);
    ImGui_ImplOpenGL2_Init();
  }

  void render() override{
    clear();
    for(auto& r : m_renderers){
      r();
    }
    drawUI();
    SDL_GL_SwapWindow(m_window);
  }

  void clear(){
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
  }
  
  void drawUI(){
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplSDL2_NewFrame(m_window);
    ImGui::NewFrame();
    
    ImGui::Begin("  ");
    ImGui::BeginTabBar("TabBar",0); 
    int count = 0;
    for(int i = 0; i < m_uiTabs.size(); i++){
      auto& handler = m_uiTabs[i];      
      std::string tabName = std::to_string(count++) + "_" + m_uiTabNames[i];
      if(ImGui::BeginTabItem(tabName.c_str())){
        handler();
        ImGui::EndTabItem();
      }
    }
    ImGui::EndTabBar();
    ImGui::End();    

    for(auto& window : m_uiWindows){
      ImGui::Begin("Window");
      window();
      ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
  }
  
  void pollInput(){
    SDL_Event event; 
    ImGuiIO& io = ImGui::GetIO();
    
    while (SDL_PollEvent(&event)){
      ImGui_ImplSDL2_ProcessEvent(&event);
      switch(event.type){
      case SDL_QUIT:
        for(auto& listener : m_eventHandlers){
          listener(window::event::Quit);
        }
        break;
      case SDL_WINDOWEVENT:
        break;
      case SDL_KEYDOWN:
        if(!io.WantCaptureKeyboard){
          for(auto& listener : m_inputHanlers){
            listener( (uint32_t) (*(SDL_GetKeyName( (event.key.keysym.sym ) ))  ));
          }
        }
        break;
      case SDL_KEYUP:
        break;
      }
    }
  }

private:
  SDL_Window*   m_window;
  SDL_Renderer* m_renderer;
  SDL_Surface*  m_surface;
  SDL_GLContext m_glContext;
};

}
}

