#pragma once
#include <glm/glm.hpp>
#include <tuple>
#include <glm/gtc/epsilon.hpp>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

namespace cgeom{
namespace intersect{

struct Result{
  glm::vec3 location;
  glm::vec3 barycentric;
  float     distance;
};

inline std::ostream& operator<<(std::ostream& out, const Result& r){
  out << glm::to_string(r.location) << " " << glm::to_string(r.barycentric) << " " << r.distance; 
  return out;
}

template<typename _T_Ray, typename _T_Plane>
float ray_plane_distance(const _T_Ray& origin, const _T_Ray& dir,
    const _T_Plane& point, const _T_Plane& normal){

  float dot = glm::dot( glm::normalize(dir), glm::normalize(normal) ) ;
  if(glm::epsilonEqual((float)fabs(dot),0.0f,0.001f)){
    return -1;
  }
  float distance = -1 * glm::dot( ( origin - point) , normal) / glm::dot(dir,normal);    
  //std::cout << "[" << glm::to_string(origin) << " " << glm::to_string(dir) << "\n" 
  //std::cout << "  "  << glm::to_string(point) << " " << glm::to_string(normal) << "]\n" << distance << std::endl << std::endl;


  return distance;
}

/**
 Intersects a ray with a triangle
 returns the u,v coorinate of the intersection as well as the distance 
 at which the intersection happened
 
 @origin the origin of the ray
 @dir the direction of the ray
 @p1 vertex1 of the triangle
 @p2 vertex2 of the triangle
 @p3 vertex3 of the triangle
 @return returns {u,v,distance}
 
 */
template<typename _T_Ray, typename _T_Face>
Result ray_tris(_T_Ray&& origin, _T_Ray&& dir,
    _T_Face&& p1,  _T_Face&& p2,  _T_Face&& p3){
   
  const glm::vec3 t1 = p2 - p1;
  const glm::vec3 t2 = p3 - p1;
  const glm::vec3 tN = glm::normalize(glm::cross(t2,t1));
  const float t = ray_plane_distance(origin,dir,p1,tN);
  if(t == -1){
    return {{},{},-1};
  }

  const glm::vec3 intersection = origin + dir * t;


  glm::mat3 mat;
  mat[0] = p1;
  mat[1] = p2;
  mat[2] = p3;
  //mat = glm::transpose(mat);
  mat = glm::inverse(mat);

  const glm::vec3 bary = mat * intersection;
  float barySum = bary[0] + bary[1] + bary[2];

  if(!glm::epsilonEqual(barySum,1.0f,0.000001f)){
    return {intersection,bary,-1};
  }
  
  if(bary[0] < 0 || bary[1] < 0 || bary[2] < 0){
    return{intersection,bary,-1};
  }
  
  Result r;
  r.location    = intersection;
  r.barycentric = bary;
  r.distance    = t; 
  return r;
}

template<typename _T_Ray, typename _T_Tris>
Result ray_tris(_T_Ray&& origin, _T_Ray&& dir, _T_Tris&& tris){
  return ray_tris(origin,dir,glm::vec3(tris[0]),glm::vec3(tris[1]),glm::vec3(tris[2]));
} 


template<typename _T_Path>
float will_collide(const _T_Path& o1, const _T_Path& v1,
    const _T_Path& o2, const _T_Path& v2){
  
  return -1;
}


};
};

