#pragma once
#include <memory>

namespace render{
namespace asset{

class Manager{
public:
  template<typename T,typename... Args>
  T* load(Args&&... args){
    T* t = new T(std::forward<Args>(args)...);    
    return t;
  }
private:


};


}
}
