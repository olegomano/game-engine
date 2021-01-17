#include  "texture.h"
#include <collections/debug_print.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>


render::asset::Texture::Texture(const std::string& path) : m_path(path){
    debug::print::print_debug("Loading Texture: ",path); 
    unsigned char *img = stbi_load(path.c_str(), &m_width, &m_height, &m_channels, 0);
    if(img == nullptr) {
      debug::print::print_error("Failed to load texture: ",path); 
      return;
    }
    m_pixels = img;
}
