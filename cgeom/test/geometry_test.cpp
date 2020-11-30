#include "gtest/gtest.h"
#include <cgeom/intersect.h>
#include <iostream>


TEST(intersect, ray_plane_0){
  glm::vec3 origin = {0,0,100};
  glm::vec3 dir = {0,0,-1};

  glm::vec3 planeOrigin = {0,0,-1};
  glm::vec3 planeNormal = {0,0,1};
  
  float result = cgeom::intersect::ray_plane_distance(origin,dir,planeOrigin,planeNormal);
  ASSERT_GT(result,0) ;
  ASSERT_FLOAT_EQ(result, 101 );

}

TEST(intersect, ray_tris_0){
  glm::vec3 origin = {0,0,100};
  glm::vec3 dir = {0,0,-1};

  glm::vec3 v1 = {-1,0,0};
  glm::vec3 v2 = {0,1,0};
  glm::vec3 v3 = {1,0,0};
  
  cgeom::intersect::Result r = cgeom::intersect::ray_tris(origin,dir,v1,v2,v3);
  //std::cout << r.x << " " << " " << r.y << " " << r.z << std::endl;
}
