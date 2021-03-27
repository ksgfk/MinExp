#include "Texture2D.h"

#include <iostream>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb_image.h>

using namespace Mine;

Texture2D::Texture2D(const std::filesystem::path& path) {
  stbi_set_flip_vertically_on_load(true);
  _data = stbi_load(path.generic_u8string().c_str(), &_width, &_height, &_channels, 0);
  if (_data == nullptr) {
    std::cout << "can't load texture " << path << "\n";
  }
}

Texture2D::~Texture2D() {
  if (_data != nullptr) {
    stbi_image_free(_data);
  }
  _data = nullptr;
}

Texture2D::Texture2D(Texture2D&& o) {
  _data = o._data;
  o._data = nullptr;
  _width = o._width;
  _height = o._height;
  _channels = o._channels;
}

Texture2D& Texture2D::operator=(Texture2D&& o) {
  _data = o._data;
  o._data = nullptr;
  _width = o._width;
  _height = o._height;
  _channels = o._channels;
  return *this;
}