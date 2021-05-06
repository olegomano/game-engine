#include "gtest/gtest.h"
#include <glm/gtx/transform.hpp>
#include <collections/stable_vector.h>
#include <collections/scene_graph.h>
#include <collections/ring_buffer.h>
#include <collections/sbo_vtable.h>
#include <collections/sync_que.h>
#include <collections/simple_que.h>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <thread>
#include <unordered_map>

TEST(ring_buffer,iterate){
  collections::ring_buffer::RingBuffer<collections::ring_buffer::ArrayWrapper<float,32>> rbuffer;
  for(int i = 0; i < rbuffer.size(); i++){
    rbuffer.push_back(i);
  }
  int count = 0;
  for(const int& i : rbuffer){
    ASSERT_EQ(i,count++);
  }
  for(int i = 0; i < rbuffer.size() * 2; i++){
    rbuffer.push_back(i);
  }
  count = rbuffer.size();
  for(const int& i : rbuffer){
    ASSERT_EQ(i,count++);
  }
}

TEST(stable_vector, insert){
  std::vector<uint32_t> vec;
  collections::stable_vector::StableVector<uint32_t> svec;
  for(int i = 0; i < svec.Capacity; i++){
    vec.push_back(i);
    svec.push_back(i);
  }
  ASSERT_EQ(svec.size(),vec.size());
  for(int i = 0; i < svec.size(); i++){
    ASSERT_EQ(svec[i],vec[i]);
  }
}


TEST(stable_vector, assert_full){
  collections::stable_vector::StableVector<uint32_t> svec;
  for(int i = 0; i < svec.Capacity; i++){
    svec.push_back(i);
  }
  ASSERT_DEATH(svec.push_back(1),".*");
}

TEST(stable_vector,iterator){
  collections::stable_vector::StableVector<uint32_t> svec;
  for(int i =0 ; i < 1024*4; i++){
    svec.push_back(i);  
  } 
  ASSERT_EQ(svec.size(), 1024*4);
  
  uint32_t count = 0;
  for(const uint32_t& i : svec){
    ASSERT_EQ(count++,i);
  }
}


TEST(scene_graph, access){
  collections::scene_graph::Scene<int> graph;
  auto c1 = graph.createNode(0);
  auto c2 = graph.createNode(1);
  auto c3 = graph.createNode(2,c1);

  ASSERT_EQ(graph[c1].data(),0);
  ASSERT_EQ(graph[c2].data(),1);
  ASSERT_EQ(graph[c3].parent(),&graph[c1]);
}


TEST(scene_graph, update){
  collections::scene_graph::Scene<int> graph;
  auto c1 = graph.createNode(0);
  auto lastNode = c1;
  int count = 0;
  while(count < 100){
    graph[lastNode].transform() = glm::translate(glm::mat4(1), glm::vec3(1.0f, 0.0f, 0.0f)); 
    ++count;
    lastNode = graph.createNode(count,lastNode);
  } 
  graph[lastNode].transform() = glm::translate(glm::mat4(1), glm::vec3(1.0f, 0.0f, 0.0f)); 

  graph.update();
  ASSERT_EQ(graph.globals().size(), count + 1);
  count = 0;
  for(const auto& globalTransform : graph.globals()){
    //ASSERT_NEAR(count++,std::get<glm::mat4>(globalTransform)[3][0],0.000001f);
    ASSERT_NEAR(++count,graph.get_transform(globalTransform)[3][0],0.000001f );
  }  
  graph.update();
}

TEST(scene_graph, copy_ctr){
  collections::scene_graph::Scene<int> scene;
  scene.createNode(1);
  scene.createNode(2);
  scene.createNode(3);
  auto parent = scene.createNode(4);
  int value = 5;
  for(int i = 0; i < 100; i++){
    parent = scene.createNode(value++,parent);
    scene.createNode(value++,parent);
  }


  collections::scene_graph::Scene<int> copy(scene);
  int count = 0;

  ASSERT_EQ(scene.nodes().size(), copy.nodes().size());
  for(const collections::scene_graph::Node<int>& node: scene.nodes()){
    if(node.hasPayload()){
      ASSERT_EQ(count,node.data());
    }
    ++count;
  }
}

TEST(scene_graph, transform){
  struct s{
    int i;
  };

  collections::scene_graph::Scene<int> scene;
  collections::scene_graph::Scene<s> dst;

  for(int i = 0; i < 100; i++){
    scene.createNode(i);
  }

  scene.morph_into<s>(dst,[&](const int& node) {
    s result;
    result.i = node;
    return result;
  });
  
  int count = 0;
  for(const auto& node : dst.nodes()){
    if(node.hasPayload()){
      std::cout << node.data().i << std::endl;
      //ASSERT_EQ(count, node.data().i);
      ++count;
    }
  }
}

TEST(sbo_vtable, create){
  struct t{
    virtual void call() = 0;
    virtual ~t(){}
  };

  struct impl : public t{  
    bool* ptr;
    void call() override {
      *ptr = true;
    }
  };
  
  bool isCalled = false;
  impl i;
  i.ptr = &isCalled;

  collections::sbo_vtable::Vtable<t, 128> interface(i);
  interface->call();
  ASSERT_EQ(isCalled,true);
}

TEST(free_list, sp_sc){
  typedef collections::sync_que::FreeList<int,256> List;
  
  List list;
  std::vector<uint16_t> nodes;

  for(int i = 0; i < 256; i++){
    uint16_t handle = list.pop_front();
    ASSERT_NE(handle,List::NULL_INDEX);
    ASSERT_EQ(handle,i);
    nodes.push_back(handle);
  }
  uint16_t fail = list.pop_front();
  ASSERT_EQ(fail,List::NULL_INDEX);
  fail = list.pop_front(); 
  ASSERT_EQ(fail,List::NULL_INDEX);

  std::function<void()> producer = [&](){
    for(int i = 0; i < nodes.size(); i++){
      uint16_t node = nodes[i];
      list[node] = i;
      list.push_back(node); 
    }
  };

  std::thread thread(producer);
  

  int32_t lastInt = 0;
  while(lastInt < 256){
    uint16_t handle = list.pop_front();
    if(handle != List::NULL_INDEX){
      ASSERT_EQ(list[handle], lastInt);
      lastInt++;
    }
  }
  thread.join();
}


template<template<class,uint32_t> typename T>
void sp_sc(){
  T<int,1024> queue;
  for(int i = 0; i < queue.capacity(); i++){
    ASSERT_EQ(true, queue.push_back(i));
  } 
  ASSERT_EQ(false,queue.push_back(0));
  for(int i = 0; i < queue.capacity(); i++){
     int value;
     ASSERT_EQ(true, queue.pop_front(value));
     ASSERT_EQ(i, value);
  }

  
   
  std::function<void()> producer = [&](){
    for(int i = 0; i < queue.capacity(); i++){
      ASSERT_EQ(true, queue.push_back(i*i));
    }    
  };
  std::thread thread(producer);
  
  int count = 0;
  while(count < queue.capacity()){
    int value;
    if(queue.pop_front(value)){
      ASSERT_EQ(count*count,value);
      ++count;
    }
  }
  
  thread.join();
}


template<template<class,uint32_t> typename T>
void mp_sc(){
  struct record{
    int threadId;
    int value;
  };

  T<record,1024> que;
  std::unordered_map<int,int> threadResults;
  std::atomic_int threadId =  {0};
  constexpr int threadCount = 4;
  constexpr int maxValue = 256*32;

  std::function<void()> producer = [&](){
    const int id = threadId.fetch_add(1);
    for(int i = 0; i < maxValue; i++){
      record r;
      r.threadId = id;
      r.value = i;
      while(!que.push_back(r)){}
      usleep(rand() % 2048);
    }
  };
  

  for(int i = 0; i < threadCount; i++){
    threadResults[i] = -1;
    std::thread t(producer);
    t.detach();
  
  }

  int doneCount = threadCount;
  while(doneCount > 0){
    record value;
    if(que.pop_front(value)){
      int lastWritten = threadResults[value.threadId];
      ASSERT_EQ(lastWritten + 1, value.value);
      threadResults[value.threadId] = value.value;
      if(value.value == maxValue - 1){
        doneCount--;
      }
    }
    usleep(rand() % 256);
  }
}

TEST(fixed_mpmc,sp_sc){
  sp_sc<collections::sync_que::FixedMpMc>();
}

TEST(fixed_mpmc,mp_sc){
  mp_sc<collections::sync_que::FixedMpMc>();
}

TEST(simple_que,sp_sc){
  sp_sc<collections::sync_que::SimpleQue>();
}

TEST(simple_que,mp_sc){
  mp_sc<collections::sync_que::SimpleQue>();
}



