#pragma once
#include <string>
#include <vector>

namespace render{
namespace asset{

class Texture{
public:
  Texture(const std::string& path);

  const uint8_t* pixels() const {
    return m_pixels;
  }

  uint32_t size() const {
    return m_width * m_width * 4;
  }

  uint32_t width() const {
    return m_width;
  }

  uint32_t height() const {
    return m_height;
  }

  uint32_t sample(float x, float y) const{ 
    x = clamp(0.0f,1.0f,x);
    y = clamp(0.0f,1.0f,y);
    uint32_t* pixels = (uint32_t*)m_pixels;
    uint32_t  xPixel = x * m_height;
    uint32_t  yPixel = y * m_width;
    uint32_t  pixel = pixels[yPixel * m_width + xPixel];
    return pixel;

  }
private:
  template<typename T>
  T clamp(T min, T max, T value) const {
    if(value < min){
      return min;
    }
    if(value > max){
      return max;
    }
    return value;
  }

  int m_width  = 0;
  int m_height = 0;
  int m_channels = 0;
  uint8_t* m_pixels = nullptr;
};



}
}
