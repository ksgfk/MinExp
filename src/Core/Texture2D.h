#pragma once

#include <filesystem>

namespace Mine {

class Texture2D {
 private:
  unsigned char* _data;
  int _width;
  int _height;
  int _channels;

 public:
  constexpr Texture2D() : _data(nullptr), _width(0), _height(0), _channels(0) {}
  Texture2D(const std::filesystem::path& path);
  ~Texture2D();
  Texture2D(const Texture2D&) = delete;
  Texture2D(Texture2D&& o);
  Texture2D& operator=(const Texture2D&) = delete;
  Texture2D& operator=(Texture2D&& o);
  constexpr const unsigned char* GetData() const { return _data; }
  constexpr int GetWidth() const { return _width; }
  constexpr int GetHeight() const { return _height; }
  constexpr int GetChannels() const { return _channels; }
};

}  // namespace Mine
