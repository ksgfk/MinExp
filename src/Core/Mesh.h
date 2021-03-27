#pragma once

#include <filesystem>
#include <vector>

#include "MathExt.h"

namespace Mine {

struct Face {
  int verticeIdx[3];
  int texcoordIdx[3];
  int normalIdx[3];
};

struct VertexAttrib {
  std::vector<Vector3> vertices;
  std::vector<Vector2> texcoords;
  std::vector<Vector3> normals;
};

struct Mesh {
  VertexAttrib attrib;
  std::vector<Face> face;
};

Mesh LoadObjFromFile(const std::filesystem::path& p);

}  // namespace Mine
