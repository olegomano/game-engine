#pragma once
#include "window.h"
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/glut.h>

namespace render{
namespace window{

class GLES2Window : public IWindow{
public:
  GLES2Window(const params& params) : IWindow(params){
    SDL_Init(SDL_INIT_VIDEO);

    m_window = SDL_CreateWindow(
      params.name.c_str(),
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      params.width,
      params.height,
      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetSwapInterval(0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    m_glContext = SDL_GL_CreateContext(m_window);
    m_renderer  = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    m_surface   = SDL_GetWindowSurface(m_window);    
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  }

  void clear(){
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  void swapBuffers(){
    SDL_GL_SwapWindow(m_window);
  }

  SDL_Window* window(){
    return m_window;
  }

  SDL_GLContext context(){
    return m_glContext;
  }
 
private:
  SDL_Window*   m_window;
  SDL_Renderer* m_renderer;
  SDL_Surface*  m_surface;
  SDL_GLContext m_glContext;
};

}
}

