#pragma once
#include<string>
#include<vector>

namespace render{
namespace gl{

class Texture{
public:
  template<typename T>
  Texture(const T& t){
     
  }
  
  template<>
  Texture<std::string>(const std::string& path){

  }

  void bind(){


  }

  void unbind(){

  }

private:
  GLuint createTexture(){
  
  }

  std::vector<uint8_t> m_buffer;
  GLuint m_handle;
};


}
}
