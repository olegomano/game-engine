#pragma once
#include <cinttypes>
#include <chrono>

namespace collections{
namespace sync_que{
/**
 * Services the que for at most time_ms microseconds
 * returns the amount of messages that were handled
 **/
template<typename _Que, typename _Function>
uint64_t drain_for(_Que&& que, _Function&& handler, uint64_t time_ms){
  auto start_time = std::chrono::high_resolution_clock::now();
  uint64_t total_time = 0;

  typename std::decay<_Que>::type::type item;
  uint64_t handled_items = 0;
  
  while(  (total_time < time_ms) ){
    if(que.pop_front(item)){
      handler(item);
    }
    total_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time).count();
    ++handled_items;
  } 
  return handled_items;
}

}
}
