/**

This File is generated by shader_gen 
DO NOT EDIT

**/
#pragma once
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
namespace render{
namespace shader{
namespace flat_color{

static constexpr char vertex_shader[186] = {0x23,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x20,0x33,0x30,0x30,0x20,0x65,0x73,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0xa,0x75,0x6e,0x69,0x66,0x6f,0x72,0x6d,0x20,0x6d,0x61,0x74,0x34,0x20,0x75,0x43,0x61,0x6d,0x65,0x72,0x61,0x3b,0xa,0x75,0x6e,0x69,0x66,0x6f,0x72,0x6d,0x20,0x6d,0x61,0x74,0x34,0x20,0x75,0x4d,0x6f,0x64,0x65,0x6c,0x4d,0x61,0x74,0x72,0x69,0x78,0x3b,0xa,0x75,0x6e,0x69,0x66,0x6f,0x72,0x6d,0x20,0x6d,0x61,0x74,0x34,0x20,0x75,0x42,0x4c,0x41,0x48,0x3b,0xa,0x69,0x6e,0x20,0x76,0x65,0x63,0x34,0x20,0x61,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x3b,0xa,0xa,0x76,0x6f,0x69,0x64,0x20,0x6d,0x61,0x69,0x6e,0x28,0x29,0x20,0x7b,0xa,0x20,0x20,0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x75,0x43,0x61,0x6d,0x65,0x72,0x61,0x20,0x2a,0x20,0x75,0x4d,0x6f,0x64,0x65,0x6c,0x4d,0x61,0x74,0x72,0x69,0x78,0x20,0x2a,0x20,0x61,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x3b,0xa,0x7d,0xa,0x00}; 
static constexpr char fragment_shader[133] = {0x23,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x20,0x33,0x30,0x30,0x20,0x65,0x73,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0xa,0x70,0x72,0x65,0x63,0x69,0x73,0x69,0x6f,0x6e,0x20,0x6d,0x65,0x64,0x69,0x75,0x6d,0x70,0x20,0x66,0x6c,0x6f,0x61,0x74,0x3b,0xa,0x75,0x6e,0x69,0x66,0x6f,0x72,0x6d,0x20,0x76,0x65,0x63,0x34,0x20,0x76,0x43,0x6f,0x6c,0x6f,0x72,0x3b,0xa,0x6f,0x75,0x74,0x20,0x76,0x65,0x63,0x34,0x20,0x66,0x72,0x61,0x67,0x43,0x6f,0x6c,0x6f,0x72,0x3b,0xa,0x76,0x6f,0x69,0x64,0x20,0x6d,0x61,0x69,0x6e,0x28,0x29,0x20,0x7b,0xa,0x20,0x20,0x66,0x72,0x61,0x67,0x43,0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,0x76,0x43,0x6f,0x6c,0x6f,0x72,0x3b,0xa,0x7d,0xa,0x00}; 

class flat_color {
public: 
  void compile(){
    GLuint vertexHandle = compileShader(vertex_shader,GL_VERTEX_SHADER);
    GLuint fragmentHandle = compileShader(fragment_shader,GL_FRAGMENT_SHADER);
    m_program = glCreateProgram();
    if(m_program == 0){
      std::cout << "Failed to compile shader" << std::endl; 
      return;
    }
    
    glAttachShader( m_program, vertexHandle ); 
    glAttachShader( m_program, fragmentHandle ); 
    glLinkProgram(m_program);
    
    GLint status = GL_FALSE;
    GLint maxLength = 0;
	  glGetShaderiv(m_program, GL_INFO_LOG_LENGTH, &maxLength);
	  std::vector<GLchar> errorLog(maxLength);

    glGetProgramiv(m_program, GL_LINK_STATUS, &status ); 
    if( GL_FALSE == status ) {
      std::cerr << "Failed to link shader program!" << std::endl;
      GLint logLen; 
      glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &logLen); 
      if( logLen > 0 ) { 
        std::string(logLen, ' ');
        GLsizei written;
        glGetProgramInfoLog(m_program, logLen, &written, &errorLog[0]); 
        std::cerr << "Program log: " << std::endl << &errorLog;
      } 
    }
    bind();
  }


  void setUniform_vColor(const float* const vector){glUniform4f(vColor,vector[0],vector[1],vector[2],vector[3]);}
	void setUniform_uCamera(const float* const matrix){glUniformMatrix4fv(uCamera, 1, GL_FALSE, matrix);}
	void setUniform_uModelMatrix(const float* const matrix){glUniformMatrix4fv(uModelMatrix, 1, GL_FALSE, matrix);}
	void setUniform_uBLAH(const float* const matrix){glUniformMatrix4fv(uBLAH, 1, GL_FALSE, matrix);}
	;
  ;
private:
  
  GLuint compileShader(const char* const shader, GLuint shaderType){
    GLuint vertShader = glCreateShader( shaderType ); 
    glShaderSource( vertShader, 1, &shader, NULL ); 
    glCompileShader( vertShader );
    
    GLint result;
    GLint maxLength = 0;
	  glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &maxLength);
	  std::vector<GLchar> errorLog(maxLength);

    glGetShaderiv( vertShader, GL_COMPILE_STATUS, &result ); 
    if( GL_FALSE == result ) { 
      std::cerr << "Vertex shader compilation failed!" << std::endl;
      GLint logLen; 
      glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLen); 
      if( logLen > 0 ) { 
        std::string log(logLen, ' '); 
        GLsizei written; 
        glGetShaderInfoLog(vertShader, logLen, &written, &errorLog[0]); 
        std::cerr << "Shader log: " << std::endl << &errorLog[0];
      } 
    }
    return vertShader;
  };

  void bind(){
    vColor = glGetUniformLocation(m_program,"vColor");
		uCamera = glGetUniformLocation(m_program,"uCamera");
		uModelMatrix = glGetUniformLocation(m_program,"uModelMatrix");
		uBLAH = glGetUniformLocation(m_program,"uBLAH");
		
  }
  GLint vColor;
	GLint uCamera;
	GLint uModelMatrix;
	GLint uBLAH;
	GLint vec;
	
  GLuint m_program;
};


}
}
}
