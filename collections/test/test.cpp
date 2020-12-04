#include "gtest/gtest.h"
#include <collections/stable_vector.h>
#include <collections/scene_graph.h>

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


TEST(scene_graph, access){
  collections::scene_graph::Scene<int> graph;
  auto c1 = graph.createNode(0);
  auto c2 = graph.createNode(1);
  auto c3 = graph.createNode(2,c1);

  ASSERT_EQ(graph[c1].data(),0);
  ASSERT_EQ(graph[c2].data(),1);
  ASSERT_EQ(graph[c3].parent(),&graph[c1]);
}


TEST(scene_graph, global_transform){

}
