#pragma once
#include <chrono>

namespace debug{
namespace timer{

class ImmidiateTimer{
public:
  template<typename T>
  ImmidiateTimer(T&& function){
    auto start = std::chrono::high_resolution_clock::now();
    function();
    auto end = std::chrono::high_resolution_clock::now();
    m_runTime = end - start;
  }

  template<typename TIME_UNIT = std::chrono::milliseconds>
  auto time() const{
    return std::chrono::duration_cast<TIME_UNIT>(m_runTime).count();
  }
  
private:
  std::chrono::nanoseconds m_runTime;
};

class PeriodTimer{
public:
  template<typename TIME_UNIT = std::chrono::milliseconds>
  auto tick(){
    auto now   = std::chrono::high_resolution_clock::now();
    auto delta = std::chrono::duration_cast<TIME_UNIT>(now - m_lastTick).count(); 
    m_lastTick = now;
    return delta;
  }

private:
  std::chrono::high_resolution_clock::time_point m_lastTick = std::chrono::high_resolution_clock::now();
};

class PipelineTimer{
public:
  void start(const std::string name){

  }

  void finish(){

  }

  const auto& time() const {
    return 1;
  }
private:


};


}
}

