#pragma once
#include <glm/glm.hpp>
#include <tuple>
#include <glm/gtc/epsilon.hpp>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

namespace cgeom{
namespace transform{


class Transform{
public:
  operator const glm::mat4&() const {
    return m_transform;
  }

  operator glm::mat4&(){
    return m_transform;
  }

  Transform(){}
  Transform(const glm::mat4& transform){
    m_transform = transform;
  }
  
  Transform(const Transform& other){
    m_transform = other.m_transform;
  }

  Transform& operator=(const glm::mat4& mat){
    m_transform = mat;
    return *this;
  }

  Transform& operator=(const Transform& t){
    m_transform =t.m_transform;
    return *this;
  }
  
  glm::mat4& transform(){
    return m_transform;
  }

  const glm::mat4& transform() const {
    return m_transform;
  }
  
  void apply(const glm::mat4& mat){
    m_transform = mat * m_transform;
  }

  void apply(const Transform& t){
    apply(t.transform());
  }

  float x() const {
    return m_transform[3][0];
  }

  float y() const {
    return m_transform[3][1];
  }

  float z() const {
    return m_transform[3][2];
  }

  const glm::vec4& position(){
    return m_transform[3];
  }
  
  void setPosition(float x, float y, float z){
    m_transform[3][0] = x;
    m_transform[3][1] = y;
    m_transform[3][2] = z;
  } 

  template<typename T>
  void setPosition(const T&& vector){
    setPosition(vector.x,vector.y,vector.z);
  }

  void translate(float x, float y, float z){
    m_transform[3][0] += x;
    m_transform[3][1] += y;
    m_transform[3][2] += z;
  }

  template<typename T>
  void translate(const T&& offset){
    translate(offset.x,offset.y,offset.z); 
  }

  template<typename AXIS>
  void rotate(float angle, const AXIS&& axis){
    m_transform = glm::rotate(m_transform,angle,glm::vec3(axis.x,axis.y,axis.z));
  }

  template<typename AXIS, typename ORIGIN>
  void rotate(float angle, const AXIS&& axis, const ORIGIN&& origin){
    ORIGIN position = {x(),y(),z()};
    ORIGIN delta = position - origin;
    translate(delta);
    rotate(angle,axis);
    translate(delta * -1);
  }

  template<typename POINT>
  void lookAt(const POINT&& p){

  }

private:
  glm::mat4 m_transform = glm::mat4(1.0f);
};


}}
