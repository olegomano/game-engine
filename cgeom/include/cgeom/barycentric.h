#pragma once
#include <glm/glm.hpp>
#include <tuple>
#include <glm/gtc/epsilon.hpp>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

namespace cgeom{
namespace intersect{


/**
 * given barycentric coords, and a uv face get the uv
 */
template<typename _T_Bary,typename _T_Face>
glm::vec2 bary_to_uv(_T_Bary&& bary, _T_Face&& face){
  glm::vec2 result;
  result.x = face[0].x*bary.x + face[1].x*bary.y + face[2].x*bary.z;
  result.y = face[0].y*bary.x + face[1].y*bary.y + face[2].y*bary.z;
  return result;
}


}
}
