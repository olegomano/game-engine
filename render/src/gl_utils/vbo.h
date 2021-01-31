#pragma once
#include <GL/glew.h>
#include <GL/glut.h>
#include <iterator>
#include <collections/debug_print.h>
#include "gl_error.h"

namespace render{
namespace gl{

class VBO{
public:
  template<typename T>
  bool allocate(const T& container){
    return allocateRaw(&container[0],std::size(container) * sizeof(float));
  }

  bool allocateRaw(const void* data,size_t bytes){
    m_size = bytes;
    debug::print::print_debug("Allocating VBO size ",bytes,"bytes");
    CheckGLError();    
    glGenBuffers(1,&m_handle);
    glBindBuffer(GL_ARRAY_BUFFER, m_handle);
    CheckGLError();    
    glBufferData(GL_ARRAY_BUFFER, bytes, NULL, GL_DYNAMIC_DRAW);
    CheckGLError();    
    glBufferSubData(GL_ARRAY_BUFFER, 0, bytes, data);
    CheckGLError();   
    glBindBuffer(GL_ARRAY_BUFFER,0);
    return true;
  }

  void bind(){
    glBindBuffer(GL_ARRAY_BUFFER, m_handle);
  }

  void unbind(){
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
  
  //amount of bytes in this vbo
  size_t size() const {
    return m_size;
  }
  
  bool destroy(){
   return true; 
  }

  operator GLuint() const {return m_handle;}
private:
  size_t m_size;
  GLuint m_handle;
};


}
}
