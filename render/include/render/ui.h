#pragma once
#include <imgui.h>
#include <string>
#include <collections/ring_buffer.h>
#include <glm/glm.hpp>
#include <collections/scene_graph.h>

namespace render{

template<typename T>
struct ui{ 
};

template<>
struct ui<glm::vec4> {
  static void draw(const glm::vec4& vec){
    if(ImGui::BeginTable("", 4)){
      ImGui::TableNextColumn(); ImGui::Text(std::to_string(vec[0]).c_str());
      ImGui::TableNextColumn(); ImGui::Text(std::to_string(vec[1]).c_str());
      ImGui::TableNextColumn(); ImGui::Text(std::to_string(vec[2]).c_str());
      ImGui::TableNextColumn(); ImGui::Text(std::to_string(vec[3]).c_str());
      ImGui::EndTable();
    } 
  }
};

template<>
struct ui<glm::mat4>{
  static void draw(const glm::mat4& mat){
    ui<glm::vec4>::draw(mat[0]);
    ui<glm::vec4>::draw(mat[1]);
    ui<glm::vec4>::draw(mat[2]);
    ui<glm::vec4>::draw(mat[3]);
  }
};

template<>
struct ui<std::string>{
  static void draw(const std::string& str){
    ImGui::Text(str.c_str()); 
  }
};

template<>
struct ui<const char*>{
  static void draw(const char*& str){
    ImGui::Text(str); 
  }
};

template<typename ArrayWrapper>
struct ui<collections::ring_buffer::RingBuffer<ArrayWrapper>>{
  static void draw(const collections::ring_buffer::RingBuffer<ArrayWrapper>& buffer){
    ImGui::PlotLines("", buffer.buffer(), buffer.size(), 
      buffer.pos(), "Frame Rate", 0.0f, 16.0f,
      ImVec2(0, 100.0f)); 
  }
};



}
