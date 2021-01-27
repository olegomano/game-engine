#include<string>
#pragma once

namespace render{
namespace gl{
namespace shader{


template<const char* _T_Name, typename T>
struct Uniform{
  static constexpr const char* name = _T_Name; 
  static void set(const T& value){
    static_assert(false,"Not Implemented For this Value");
  }
};

template<typename _T_UniformTuple, typename _T_AttributeTuple>
class Shader{
public:
  Shader(const std::string& fragment, const std::string& vertex){

  };

  template<const char* _T_Name>
  auto uniform(){

  }

  template<const char* _T_Name>
  auto attribute(){

  }
private:
  void bind();
};



}
}
}
