#include <benchmark/benchmark.h>
#include <collections/sync_que.h>
#include <collections/simple_que.h>
#include <thread>


template<typename Container>
static void sp_sc(uint32_t count){
  Container que;
  auto producer = [&](){
    typename Container::type data = {};
    for(int i = 0; i < count; i++){
      while(!que.push_back(data)){}
    }  
  };
  std::thread t(producer);
  
  uint32_t processed = 0;
  typename Container::type result;
  while(processed < count){
    while(!que.pop_front(result)){}
    ++processed;
  }
  t.join();
}

template<typename Container>
static void mp_sc(uint32_t count, uint32_t tcount,uint64_t sleep){
  Container que;
  
  auto producer = [&](){
    typename Container::type data = {};
    for(int i = 0; i < count; i++){
      while(!que.push_back(data)){}
      std::this_thread::sleep_for(std::chrono::microseconds(sleep*2));
    }  
  };
  for(int i = 0; i < tcount; i++){
    std::thread t(producer);
    t.detach();
  }

  
  uint32_t processed = 0;
  typename Container::type result;
  while(processed < count*tcount){
    while(!que.pop_front(result)){}
    ++processed;
  }
}


template<template<class,uint32_t> typename T>
static void BM_sp_sc(benchmark::State& state){
  struct buffer{
    char c[8];
  };

  for(auto _ : state){
    sp_sc<T<buffer,3024>>(1000);
  }
};

template<template<class,uint32_t> typename T>
static void BM_mp_sc(benchmark::State& state){
  struct buffer{
    std::string a = "1111122222233333334444445";
    std::string b = "1111122222233333334444445";
    std::string c = "1111122222233333334444445";
  };

  for(auto _ : state){
    mp_sc<T<buffer,2048>>(2024*8,8,6);
  }
};



BENCHMARK_TEMPLATE(BM_sp_sc,collections::sync_que::FixedMpMc);
BENCHMARK_TEMPLATE(BM_mp_sc,collections::sync_que::FixedMpMc);
BENCHMARK_TEMPLATE(BM_sp_sc,collections::sync_que::SimpleQue);
BENCHMARK_TEMPLATE(BM_mp_sc,collections::sync_que::SimpleQue);
BENCHMARK_MAIN();


