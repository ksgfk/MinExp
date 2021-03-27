#pragma once

#include <glad/glad.h>
#include <filesystem>
#include <map>
#include <variant>
#include <memory>
#include <functional>

#include "Define.h"
#include "Mesh.h"
#include "Texture2D.h"
#include "Light.h"

#ifdef MINE_DEBUG
#define MineGLFuncCall(Func) \
  Func;                      \
  Mine::LogGLFuncCall(__FILE__, __LINE__);
#else
#define MineGLFuncCall(Func) Func;
#endif

namespace Mine {

void LogGLFuncCall(const char* filename, int lineNum);

struct VertexAttribDescOpenGL {
  GLuint index;
  GLint size;
  GLenum type;
  GLsizei stride;
  size_t offset;
};

struct GPUMeshDescOpenGL {
  std::vector<VertexAttribDescOpenGL> attribDesc;
  std::vector<float> data;
  std::vector<unsigned int> indices;
};

class GPUBufferOpenGL {
 private:
  GLuint _handle;
  GLsizeiptr _size;
  GLenum _target;
  GLenum _usage;

 public:
  GPUBufferOpenGL();
  GPUBufferOpenGL(GLenum target, GLenum usage, const void* data, GLsizeiptr size);
  GPUBufferOpenGL(const GPUBufferOpenGL&) = delete;
  GPUBufferOpenGL(GPUBufferOpenGL&& o) noexcept;
  ~GPUBufferOpenGL();
  GPUBufferOpenGL& operator=(const GPUBufferOpenGL&) = delete;
  GPUBufferOpenGL& operator=(GPUBufferOpenGL&& o) noexcept;
  constexpr GLenum GetTarget() const { return _target; }
  constexpr GLenum GetUsage() const { return _usage; }
  constexpr GLsizeiptr GetSize() const { return _size; }
  void Bind() const;
  void Delete();
};

class GPUMeshOpenGL {
 private:
  GLuint _vao;
  GPUBufferOpenGL _vbo;
  GPUBufferOpenGL _ebo;

 public:
  GPUMeshOpenGL();
  GPUMeshOpenGL(const GPUMeshDescOpenGL& desc);
  GPUMeshOpenGL(const GPUMeshOpenGL&) = delete;
  GPUMeshOpenGL(GPUMeshOpenGL&& o) noexcept;
  ~GPUMeshOpenGL();
  GPUMeshOpenGL& operator=(const GPUMeshOpenGL&) = delete;
  GPUMeshOpenGL& operator=(GPUMeshOpenGL&& o) noexcept;
  void Bind() const;
  void Delete();
  GLsizei GetIndexCount() const;
};

struct ShaderUniformDescOpenGL {
  std::string name;
  GLenum type;
  GLint location;
  GLint count;
};

using UniformDescMapOpenGL = std::map<std::string, ShaderUniformDescOpenGL>;
using UniformObjectOpenGL = std::variant<int, float, Vector3, Matrix4x4>;
using UniformMapOpenGL = std::map<std::string_view, UniformObjectOpenGL>;

class ShaderProgramOpenGL {
 private:
  GLuint _handle;
  UniformDescMapOpenGL _uniformDesc;

 public:
  ShaderProgramOpenGL();
  ShaderProgramOpenGL(std::string_view vs, std::string_view fs);
  ShaderProgramOpenGL(const ShaderProgramOpenGL&) = delete;
  ShaderProgramOpenGL(ShaderProgramOpenGL&& o) noexcept;
  ~ShaderProgramOpenGL();
  ShaderProgramOpenGL& operator=(const ShaderProgramOpenGL&) = delete;
  ShaderProgramOpenGL& operator=(ShaderProgramOpenGL&& o) noexcept;
  void Bind() const;
  void Delete();
  const UniformDescMapOpenGL& GetUniformDesc() const;
  void SetPass(const UniformMapOpenGL& uniform) const;
};

class ShaderUniformOpenGL {
 public:
  static UniformObjectOpenGL CreateUniformObject(GLenum type);

 private:
  UniformMapOpenGL _objectMap;

 public:
  ShaderUniformOpenGL();
  ShaderUniformOpenGL(const UniformDescMapOpenGL& map);
  ShaderUniformOpenGL(const ShaderUniformOpenGL& o);
  ShaderUniformOpenGL(ShaderUniformOpenGL&& o);
  ~ShaderUniformOpenGL();
  ShaderUniformOpenGL& operator=(const ShaderUniformOpenGL& o);
  ShaderUniformOpenGL& operator=(ShaderUniformOpenGL&& o);
  template <typename T>
  void SetValue(std::string_view name, T value) {
    auto iter = _objectMap.find(name);
    if (iter == _objectMap.end()) {
      return;
    }
    iter->second.emplace<T>(value);
  }
  const UniformMapOpenGL& GetUniformObjects() const;
};

class MeshRendererOpenGL {
 public:
  std::weak_ptr<ShaderProgramOpenGL> shader;
  std::weak_ptr<ShaderUniformOpenGL> material;
  std::weak_ptr<GPUMeshOpenGL> mesh;

 public:
  MeshRendererOpenGL();
  MeshRendererOpenGL(const MeshRendererOpenGL& o);
  MeshRendererOpenGL(MeshRendererOpenGL&& o);
  ~MeshRendererOpenGL();
  MeshRendererOpenGL& operator=(const MeshRendererOpenGL& o);
  MeshRendererOpenGL& operator=(MeshRendererOpenGL&& o);
  void Render() const;
};

struct GPUTexture2DDescOpenGL {
  /*
   * GL_REPEAT
   * GL_MIRRORED_REPEAT
   * GL_CLAMP_TO_EDGE
   */
  GLint wrapS;
  GLint wrapT;

  /*
   * GL_NEAREST
   * GL_LINEAR
   * GL_LINEAR_MIPMAP_LINEAR (only minFliter)
   */
  GLint minFliter;
  GLint magFliter;
  GLint mipmapLevel;
  GLint format;
  GLsizei width;
  GLsizei height;
  GLenum dataFormat;
  GLenum dataType;
  GLvoid* dataPtr;
};

class GPUTexture2DOpenGL {
 private:
  GLuint _handle;

 public:
  GPUTexture2DOpenGL();
  GPUTexture2DOpenGL(const GPUTexture2DDescOpenGL& desc);
  GPUTexture2DOpenGL(const GPUTexture2DOpenGL&) = delete;
  GPUTexture2DOpenGL(GPUTexture2DOpenGL&& o);
  ~GPUTexture2DOpenGL();
  GPUTexture2DOpenGL& operator=(const GPUTexture2DOpenGL&) = delete;
  GPUTexture2DOpenGL& operator=(GPUTexture2DOpenGL&& o);
  void Bind(GLenum id) const;
  void Delete();
};

void InitOpenGL(int width, int height, const char* title);
void TerminateOpenGL();
bool ShouldTerminateOpenGL();
void* GetNativeWindowOpenGL();
void SetFrameBufferResizeCallbackOpenGL(std::function<void(int, int)> callback);
std::pair<int, int> GetFrameBufferSizeOpenGL();

std::shared_ptr<GPUMeshOpenGL> CreateMeshBufferOpenGL(const Mesh& mesh);
std::shared_ptr<ShaderProgramOpenGL> CreateShaderProgramOpenGL(const std::filesystem::path& path);
std::shared_ptr<ShaderUniformOpenGL> CreateShaderUniformOpenGL(const ShaderProgramOpenGL& shader);
std::shared_ptr<GPUTexture2DOpenGL> CreateTexture2DOpenGL(const GPUTexture2DDescOpenGL& desc);
std::shared_ptr<GPUTexture2DOpenGL> CreateTexture2DOpenGL(const Texture2D& tex2d);
void SetPointLightOpenGL(ShaderUniformOpenGL& material, const PointLight& light, const Vector3& camPos);

}  // namespace Mine