#include "Mesh.h"

#include <fstream>
#include <string>
#include <cstring>
#include <cassert>

using namespace Mine;

static void _ParseObj(const char* line,
                      std::vector<Vector3>& pos,
                      std::vector<Vector2>& tex,
                      std::vector<Vector3>& nor,
                      std::vector<Face>& fac) {
  if (line[0] == '#') {
    return;
  }
  if (strncmp(line, "v ", 2) == 0) {  //position
    Vector3 v;
    int cnt = sscanf(line, "v %f %f %f", &v.x, &v.y, &v.z);
    assert(cnt == 3);
    pos.emplace_back(v);
  } else if (strncmp(line, "vt ", 3) == 0) {  //texcoord
    Vector2 vt;
    int cnt = sscanf(line, "vt %f %f", &vt.x, &vt.y);
    assert(cnt == 2);
    tex.emplace_back(vt);
  } else if (strncmp(line, "vn ", 3) == 0) {  //normal
    Vector3 vn;
    int cnt = sscanf(line, "vn %f %f %f", &vn.x, &vn.y, &vn.z);
    assert(cnt == 3);
    nor.emplace_back(vn);
  } else if (strncmp(line, "f ", 2) == 0) {  //face
    Face f;
    int cnt = sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                     &f.verticeIdx[0], &f.texcoordIdx[0], &f.normalIdx[0],
                     &f.verticeIdx[1], &f.texcoordIdx[1], &f.normalIdx[1],
                     &f.verticeIdx[2], &f.texcoordIdx[2], &f.normalIdx[2]);
    assert(cnt == 9);
    for (auto& v : f.verticeIdx) {
      v -= 1;
    }
    for (auto& t : f.texcoordIdx) {
      t -= 1;
    }
    for (auto& n : f.normalIdx) {
      n -= 1;
    }
    fac.emplace_back(f);
  }
}

Mesh Mine::LoadObjFromFile(const std::filesystem::path& p) {
  std::ifstream fs(p.generic_u8string() + ".obj", std::ios::in);
  if (!fs.is_open()) {
    throw "can't load obj";
  }

  std::vector<Vector3> pos;
  std::vector<Vector2> tex;
  std::vector<Vector3> nor;
  std::vector<Face> fac;

  std::string linebuf;
  while (fs.peek() != -1) {
    std::getline(fs, linebuf);
    const char* line = linebuf.c_str();
    if (linebuf.empty()) {
      continue;
    }
    _ParseObj(line, pos, tex, nor, fac);
  }

  pos.shrink_to_fit();
  tex.shrink_to_fit();
  nor.shrink_to_fit();
  fac.shrink_to_fit();

  Mesh m;
  m.attrib.vertices = std::move(pos);
  m.attrib.texcoords = std::move(tex);
  m.attrib.normals = std::move(nor);
  m.face = std::move(fac);
  return m;
}

MultiMesh Mine::LoadObjWithChildFromFile(const std::filesystem::path& p) {
  std::ifstream fs(p.generic_u8string() + ".obj", std::ios::in);
  if (!fs.is_open()) {
    throw "can't load obj";
  }
  std::vector<Vector3> pos;
  std::vector<Vector2> tex;
  std::vector<Vector3> nor;
  MultiMesh::ObjArray faces;

  MultiMesh::FaceArray tempFace;
  std::string tempName;

  std::string linebuf;
  while (fs.peek() != -1) {
    std::getline(fs, linebuf);
    const char* line = linebuf.c_str();
    if (linebuf.empty()) {
      continue;
    }
    _ParseObj(line, pos, tex, nor, tempFace);
    if (strncmp(line, "usemtl ", 6) == 0) {
      char name[256];
      int cnt = sscanf(line, "usemtl %s", name);
      assert(cnt == 1);
      if (!tempFace.empty()) {
        tempFace.shrink_to_fit();
        faces.emplace_back(std::make_pair(std::move(tempName), std::move(tempFace)));
        tempFace = MultiMesh::FaceArray();
      }
      tempName = std::string(name);
    }
  }

  if (!tempFace.empty()) {
    tempFace.shrink_to_fit();
    faces.emplace_back(std::make_pair(std::move(tempName), std::move(tempFace)));
  }

  pos.shrink_to_fit();
  tex.shrink_to_fit();
  nor.shrink_to_fit();
  faces.shrink_to_fit();
  MultiMesh m;
  m.attrib.vertices = std::move(pos);
  m.attrib.texcoords = std::move(tex);
  m.attrib.normals = std::move(nor);
  m.obj = std::move(faces);
  return m;
}