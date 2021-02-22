#pragma once
#include <cassert>

namespace collections{
namespace sbo_vtable{

template<typename _T_Interface, uint32_t _T_Size>
class Vtable{
public:
  static constexpr uint32_t size = _T_Size;
  Vtable(){
    
  }
  
  template<uint32_t Size>
  Vtable(const Vtable<_T_Interface,Size>& other){
    static_assert(Size <= _T_Size, "Can only copy into bigger Vtable");
    memcpy(m_buffer,other.m_buffer,other.size);
  }

  template<typename _T_Instance>
  Vtable(const _T_Instance& i){
    static_assert(sizeof(_T_Instance) < _T_Size, "Object is too large to be converted into Vtable");
    _T_Instance* ptr = (_T_Instance*) (&m_buffer[0]);
    new(m_buffer) _T_Instance(i);
    m_null = false;
  }

  template<typename _T_Instance>
  Vtable(const _T_Instance* const i){
    static_assert(sizeof(_T_Instance) < _T_Size, "Object is too large to be converted into Vtable"); 
     _T_Instance* ptr = (_T_Instance*) (&m_buffer[0]);
    new(m_buffer) _T_Instance(*i);
    m_null = false;
  }

  _T_Interface* operator ->(){
    assert(!m_null);
    return (_T_Interface*)(m_buffer);
  }

  const _T_Interface* operator ->() const {
    assert(!m_null);
    return (_T_Interface*)(m_buffer);
  }

  operator bool() const {
    return m_null;
  }

  operator _T_Interface*() const {
    assert(!m_null);
    return (_T_Interface*)(m_buffer);
  }
private:
  uint8_t m_buffer[_T_Size];
  bool m_null = true;
};


}
}
