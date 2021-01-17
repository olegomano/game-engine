#include "window.h"
#include <SDL2/SDL.h>

void render::window::SDLInputManager::pollInput(){
  SDL_Event event; 
  while (SDL_PollEvent(&event)){
    switch(event.type){
    case SDL_QUIT:
      sendEvent(event::Quit);
      break;
    case SDL_WINDOWEVENT:
      break;
    case SDL_KEYDOWN:
        sendKeyEvent( (uint32_t) (*(SDL_GetKeyName( (event.key.keysym.sym ) ))  ));
      break;
    case SDL_KEYUP:
      break;
    }
  }
}





