#include "gtest/gtest.h"
#include <glm/gtx/transform.hpp>
#include <collections/stable_vector.h>
#include <collections/scene_graph.h>
#include <collections/ring_buffer.h>
#include <glm/gtx/string_cast.hpp>
#include <iostream>


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
