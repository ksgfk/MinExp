#include "OpenGLContext.h"

#include <iostream>
#include <fstream>
#include <string>
#include <streambuf>
#include <cassert>

using namespace Mine;

static void _DefaultGLError(GLenum source,
                            GLenum type,
                            GLuint id,
                            GLenum severity,
                            GLsizei length,
                            const GLchar* message,
                            const void* userParam) {
  std::cout << "[";
  switch (source) {
    case GL_DEBUG_SOURCE_API:
      std::cout << "API";
      break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      std::cout << "WindowSystem";
      break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      std::cout << "ShaderCompiler";
      break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      std::cout << "ThirdParty";
      break;
    case GL_DEBUG_SOURCE_APPLICATION:
      std::cout << "Application";
      break;
    case GL_DEBUG_SOURCE_OTHER:
      std::cout << "Other";
      break;
    default:
      break;
  }
  std::cout << "][";
  switch (type) {
    case GL_DEBUG_TYPE_ERROR:
      std::cout << "Error";
      break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      std::cout << "DeprecatedBehaviour";
      break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      std::cout << "UndefinedBehaviour";
      break;
    case GL_DEBUG_TYPE_PORTABILITY:
      std::cout << "Portability";
      break;
    case GL_DEBUG_TYPE_PERFORMANCE:
      std::cout << "Performance";
      break;
    case GL_DEBUG_TYPE_MARKER:
      std::cout << "Marker";
      break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
      std::cout << "PushGroup";
      break;
    case GL_DEBUG_TYPE_POP_GROUP:
      std::cout << "PopGroup";
      break;
    case GL_DEBUG_TYPE_OTHER:
      std::cout << "Other";
      break;
    default:
      break;
  }
  std::cout << "][";
  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
      std::cout << "High";
      break;
    case GL_DEBUG_SEVERITY_MEDIUM:
      std::cout << "Medium";
      break;
    case GL_DEBUG_SEVERITY_LOW:
      std::cout << "Low";
      break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      std::cout << "Note";
      break;
    default:
      break;
  }
  std::cout << "][" << id << "]:" << message << "\n";
}

#ifdef MINE_PLATFORM_WIN32
#include <GLFW/glfw3.h>

static GLFWwindow* _window;
static std::function<void(int, int)> _framebufferResize;

static void _OnFrameBufferResize(GLFWwindow* window, int width, int height) {
  if (_framebufferResize) {
    _framebufferResize(width, height);
  }
}

void Mine::InitOpenGL(int width, int height, const char* title) {
  if (_window != nullptr) {
    return;
  }
#ifdef MINE_DEBUG
  glfwSetErrorCallback([](int errorCode, const char* info) -> void { std::cout << "glfw:" << errorCode << ":" << info << "\n"; });
#endif
  if (glfwInit() != GLFW_TRUE) {
    std::cout << "Can't init glfw\n";
    return;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#ifdef MINE_DEBUG
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  _window = glfwCreateWindow(width, height, title, nullptr, nullptr);
  glfwMakeContextCurrent(_window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Can't load OpenGL context\n";
    glfwDestroyWindow(_window);
    _window = nullptr;
    return;
  }
#ifdef MINE_DEBUG
  MineGLFuncCall(glDebugMessageCallback(_DefaultGLError, nullptr));
#endif
  glfwSetFramebufferSizeCallback(_window, _OnFrameBufferResize);
}

void Mine::TerminateOpenGL() {
  glfwTerminate();
  _window = nullptr;
}

bool Mine::ShouldTerminateOpenGL() {
  glfwSwapBuffers(_window);
  glfwPollEvents();
  return glfwWindowShouldClose(_window);
}

void* Mine::GetNativeWindowOpenGL() {
  return _window;
}

void Mine::SetFrameBufferResizeCallbackOpenGL(std::function<void(int, int)> callback) {
  _framebufferResize = callback;
}

std::pair<int, int> Mine::GetFrameBufferSizeOpenGL() {
  int w, h;
  glfwGetFramebufferSize(_window, &w, &h);
  return std::make_pair(w, h);
}

#endif

GPUBufferOpenGL::GPUBufferOpenGL() : _handle(0), _size(0), _target(0), _usage(0) {}

GPUBufferOpenGL::GPUBufferOpenGL(GLenum target, GLenum usage, const void* data, GLsizeiptr size) {
  MineGLFuncCall(glGenBuffers(1, &_handle));
  MineGLFuncCall(glBindBuffer(target, _handle));
  MineGLFuncCall(glBufferData(target, size, data, usage));
  _size = size;
  _target = target;
  _usage = usage;
}

GPUBufferOpenGL::GPUBufferOpenGL(GPUBufferOpenGL&& o) noexcept {
  _handle = o._handle;
  o._handle = 0;
  _size = o._size;
  _target = o._target;
  _usage = o._usage;
}

GPUBufferOpenGL::~GPUBufferOpenGL() {
  Delete();
}

GPUBufferOpenGL& GPUBufferOpenGL::operator=(GPUBufferOpenGL&& o) noexcept {
  _handle = o._handle;
  o._handle = 0;
  _size = o._size;
  _target = o._target;
  _usage = o._usage;
  return *this;
}

void GPUBufferOpenGL::Bind() const {
  MineGLFuncCall(glBindBuffer(_target, _handle));
}

void GPUBufferOpenGL::Delete() {
  if (_handle != 0) {
    MineGLFuncCall(glDeleteBuffers(1, &_handle));
  }
  _handle = 0;
}

void Mine::LogGLFuncCall(const char* filename, int lineNum) {
  for (GLenum err = 0; (err = glGetError()) != GL_NO_ERROR;) {
    switch (err) {
      case GL_INVALID_ENUM:
        std::cout << filename << " line[" << lineNum << "]:Invalid enum.\n";
        break;
      case GL_INVALID_VALUE:
        std::cout << filename << " line[" << lineNum << "]:Invalid value.\n";
        break;
      case GL_INVALID_OPERATION:
        std::cout << filename << " line[" << lineNum << "]:Invalid operation.\n";
        break;
      case GL_OUT_OF_MEMORY:
        std::cout << filename << " line[" << lineNum << "]:Out of memory.\n";
        break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        std::cout << filename << " line[" << lineNum << "]:Invalid FrameBuffer operation.\n";
        break;
      default:
        break;
    }
  }
}

GPUMeshOpenGL::GPUMeshOpenGL() : _vao(0), _vbo(), _ebo() {}

GPUMeshOpenGL::GPUMeshOpenGL(const GPUMeshDescOpenGL& desc) {
  _vbo = GPUBufferOpenGL(GL_ARRAY_BUFFER, GL_STATIC_DRAW, desc.data.data(), desc.data.size() * sizeof(float));
  _vbo.Bind();
  MineGLFuncCall(glGenVertexArrays(1, &_vao));
  MineGLFuncCall(glBindVertexArray(_vao));
  for (const auto& d : desc.attribDesc) {
    MineGLFuncCall(glVertexAttribPointer(d.index, d.size, d.type, GL_FALSE, d.stride, (void*)(d.offset)));
    MineGLFuncCall(glEnableVertexAttribArray(d.index));
  }
  MineGLFuncCall(glBindVertexArray(0));
  _ebo = GPUBufferOpenGL(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, desc.indices.data(), desc.indices.size() * sizeof(unsigned int));
}

GPUMeshOpenGL::GPUMeshOpenGL(GPUMeshOpenGL&& o) noexcept {
  _vao = o._vao;
  o._vao = 0;
  _vbo = std::move(o._vbo);
  _ebo = std::move(o._ebo);
}

GPUMeshOpenGL::~GPUMeshOpenGL() {
  Delete();
}

GPUMeshOpenGL& GPUMeshOpenGL::operator=(GPUMeshOpenGL&& o) noexcept {
  _vao = o._vao;
  o._vao = 0;
  _vbo = std::move(o._vbo);
  _ebo = std::move(o._ebo);
  return *this;
}
void GPUMeshOpenGL::Bind() const {
  _vbo.Bind();
  MineGLFuncCall(glBindVertexArray(_vao));
  _ebo.Bind();
}

void GPUMeshOpenGL::Delete() {
  if (_vao != 0) {
    MineGLFuncCall(glDeleteVertexArrays(1, &_vao));
  }
  _vao = 0;
  _vbo.Delete();
  _ebo.Delete();
}

GLsizei GPUMeshOpenGL::GetIndexCount() const {
  return (GLsizei)(_ebo.GetSize() / sizeof(unsigned int));
}

struct _Temp {
  int v, t, n;
};
bool operator<(const _Temp& a, const _Temp& b) { return a.v == b.v ? (a.t == b.t ? a.n < b.n : a.t < b.t) : a.v < b.v; }

std::shared_ptr<GPUMeshOpenGL> Mine::CreateMeshBufferOpenGL(const Mesh& mesh, bool hasNormal, bool hasTexcoord) {
  std::vector<float> buffer;
  std::vector<unsigned int> indice;
  std::map<_Temp, unsigned int> cull;
  unsigned int id = 0;
  for (const Face& f : mesh.face) {
    for (int i = 0; i < 3; i++) {
      _Temp t{f.verticeIdx[i], f.texcoordIdx[i], f.normalIdx[i]};
      auto iter = cull.find(t);
      if (iter == cull.end()) {
        const auto& p = mesh.attrib.vertices[f.verticeIdx[i]];
        const auto& t = mesh.attrib.texcoords[f.texcoordIdx[i]];
        const auto& n = mesh.attrib.normals[f.normalIdx[i]];
        buffer.emplace_back(p.x);
        buffer.emplace_back(p.y);
        buffer.emplace_back(p.z);
        if (hasTexcoord) {
          buffer.emplace_back(t.x);
          buffer.emplace_back(t.y);
        }
        if (hasNormal) {
          buffer.emplace_back(n.x);
          buffer.emplace_back(n.y);
          buffer.emplace_back(n.z);
        }
        if (hasTexcoord && hasNormal) {
          cull.emplace(_Temp{f.verticeIdx[i], f.texcoordIdx[i], f.normalIdx[i]}, id);
        }
        if (!hasTexcoord && hasNormal) {
          cull.emplace(_Temp{f.verticeIdx[i], -1, f.normalIdx[i]}, id);
        }
        if (hasTexcoord && !hasNormal) {
          cull.emplace(_Temp{f.verticeIdx[i], f.texcoordIdx[i], -1}, id);
        }
        if (!hasTexcoord && !hasNormal) {
          cull.emplace(_Temp{f.verticeIdx[i], -1, -1}, id);
        }
        indice.emplace_back(id);
        id++;
      } else {
        indice.emplace_back(iter->second);
      }
    }
  }
  GPUMeshDescOpenGL desc;
  desc.data = std::move(buffer);
  desc.indices = std::move(indice);
  GLsizei stride = (GLsizei)(sizeof(Vector3) + (hasTexcoord ? sizeof(Vector2) : 0) + (hasNormal ? sizeof(Vector3) : 0));
  auto posOffset = sizeof(Vector3);
  auto texOffset = sizeof(Vector3) + sizeof(Vector2);
  desc.attribDesc.emplace_back(VertexAttribDescOpenGL{0, 3, GL_FLOAT, stride, 0});  //pos
  if (hasTexcoord) {
    desc.attribDesc.emplace_back(VertexAttribDescOpenGL{1, 2, GL_FLOAT, stride, posOffset});  //texcoord
  }
  if (hasNormal) {
    desc.attribDesc.emplace_back(VertexAttribDescOpenGL{2, 3, GL_FLOAT, stride, texOffset});  //normal
  }
  return std::make_shared<GPUMeshOpenGL>(desc);
}

static GLuint _ComplierShader(GLenum type, std::string_view src) {
  auto s = MineGLFuncCall(glCreateShader(type));
  auto dp = src.data();
  MineGLFuncCall(glShaderSource(s, 1, &dp, nullptr));
  MineGLFuncCall(glCompileShader(s));
  GLint success;
  MineGLFuncCall(glGetShaderiv(s, GL_COMPILE_STATUS, &success));
#ifdef MINE_DEBUG
  if (success != GL_TRUE) {
    int len;
    MineGLFuncCall(glGetShaderiv(s, GL_INFO_LOG_LENGTH, &len));
    std::string info(len, '\0');
    MineGLFuncCall(glGetShaderInfoLog(s, len, nullptr, info.data()));
    std::cout << info << "\n";
  }
#endif
  if (success != GL_TRUE) {
    MineGLFuncCall(glDeleteShader(s));
    s = 0;
  }
  return s;
}

ShaderProgramOpenGL::ShaderProgramOpenGL() : _handle(0) {}

static UniformDescMapOpenGL _GetShaderUniformDesc(GLuint program) {
  UniformDescMapOpenGL map;
  GLint activeUniformLength;
  MineGLFuncCall(glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &activeUniformLength));
  for (auto i = 0; i < activeUniformLength; i++) {
    const int bufferSize = 512;
    char nameBuffer[bufferSize];
    GLsizei nameSize;
    GLint count;
    GLenum type;
    MineGLFuncCall(glGetActiveUniform(program, i, bufferSize, &nameSize, &count, &type, nameBuffer));
    std::string name(nameBuffer);
    if (count > 1) {
      auto pos = name.find("[0]", 0);
      name.replace(pos, 3, "");
    }
    ShaderUniformDescOpenGL desc;
    desc.name = name;
    desc.type = type;
    desc.location = MineGLFuncCall(glGetUniformLocation(program, nameBuffer));
    desc.count = count;
    map.emplace(std::move(name), desc);
  }
  return map;
}

ShaderProgramOpenGL::ShaderProgramOpenGL(std::string_view vs, std::string_view fs) {
  auto vss = _ComplierShader(GL_VERTEX_SHADER, vs);
  auto fss = _ComplierShader(GL_FRAGMENT_SHADER, fs);
  _handle = MineGLFuncCall(glCreateProgram());
  MineGLFuncCall(glAttachShader(_handle, vss));
  MineGLFuncCall(glAttachShader(_handle, fss));
  MineGLFuncCall(glLinkProgram(_handle));
  GLint success;
  MineGLFuncCall(glGetProgramiv(_handle, GL_LINK_STATUS, &success));
#ifdef MINE_DEBUG
  if (success != GL_TRUE) {
    int len;
    MineGLFuncCall(glGetProgramiv(_handle, GL_INFO_LOG_LENGTH, &len));
    std::string info(len, '\0');
    MineGLFuncCall(glGetProgramInfoLog(_handle, len, nullptr, info.data()));
    std::cout << info << "\n";
  }
#endif
  if (success != GL_TRUE) {
    MineGLFuncCall(glDeleteProgram(_handle));
    _handle = 0;
  }
  _uniformDesc = _GetShaderUniformDesc(_handle);
}

ShaderProgramOpenGL::ShaderProgramOpenGL(ShaderProgramOpenGL&& o) noexcept {
  _handle = o._handle;
  o._handle = 0;
  _uniformDesc = std::move(o._uniformDesc);
}

ShaderProgramOpenGL::~ShaderProgramOpenGL() {
  Delete();
}

ShaderProgramOpenGL& ShaderProgramOpenGL::operator=(ShaderProgramOpenGL&& o) noexcept {
  _handle = o._handle;
  o._handle = 0;
  _uniformDesc = std::move(o._uniformDesc);
  return *this;
}

void ShaderProgramOpenGL::Bind() const { MineGLFuncCall(glUseProgram(_handle)); }

void ShaderProgramOpenGL::Delete() {
  if (_handle != 0) {
    MineGLFuncCall(glDeleteProgram(_handle));
  }
  _handle = 0;
}

const UniformDescMapOpenGL& ShaderProgramOpenGL::GetUniformDesc() const { return _uniformDesc; }

void ShaderProgramOpenGL::SetPass(const UniformMapOpenGL& uniform) const {
  Bind();
  auto localIter = _uniformDesc.begin();
  auto uniformIter = uniform.begin();
  for (; localIter != _uniformDesc.end() && uniformIter != uniform.end(); localIter++, uniformIter++) {
    assert(localIter->first == uniformIter->first);
    const auto& desc = localIter->second;
    if (desc.count == 1) {
      switch (desc.type) {
        case GL_FLOAT:
          MineGLFuncCall(glUniform1f(desc.location, std::get<float>(uniformIter->second)));
          break;
        case GL_INT:
          MineGLFuncCall(glUniform1i(desc.location, std::get<int>(uniformIter->second)));
          break;
        case GL_FLOAT_VEC3:
          MineGLFuncCall(glUniform3fv(desc.location, desc.count, &std::get<Vector3>(uniformIter->second).x));
          break;
        case GL_FLOAT_MAT4:
          MineGLFuncCall(glUniformMatrix4fv(desc.location, desc.count, GL_FALSE, &std::get<Matrix4x4>(uniformIter->second).m11));
          break;
        case GL_SAMPLER_2D:
          MineGLFuncCall(glUniform1i(desc.location, std::get<int>(uniformIter->second)));
          break;
        default:
          throw "unsupported type";
      }
    } else {
      const auto& uniObj = uniformIter->second;
      switch (desc.type) {
        case GL_FLOAT_MAT4:
          MineGLFuncCall(glUniformMatrix4fv(desc.location,
                                            desc.count,
                                            GL_FALSE,
                                            (GLfloat*)std::get<UniformArrayObjectOpenGL<Matrix4x4>>(uniObj).data()));
          break;
        case GL_SAMPLER_2D:
          MineGLFuncCall(glUniform1iv(desc.location, desc.count, std::get<UniformArrayObjectOpenGL<int>>(uniObj).data()));
        default:
          throw "unsupported type";
      }
    }
  }
}

std::shared_ptr<ShaderProgramOpenGL> Mine::CreateShaderProgramOpenGL(const std::filesystem::path& path) {
  auto vsPath = path.generic_u8string() + ".vert";
  auto fsPath = path.generic_u8string() + ".frag";
  std::ifstream vsif(vsPath, std::ios::in);
  std::string vsSrc = std::string(std::istreambuf_iterator<char>(vsif), std::istreambuf_iterator<char>());
  vsif.close();
  std::ifstream fsif(fsPath, std::ios::in);
  std::string fsSrc = std::string(std::istreambuf_iterator<char>(fsif), std::istreambuf_iterator<char>());
  fsif.close();
  return std::make_shared<ShaderProgramOpenGL>(vsSrc, fsSrc);
}

ShaderUniformOpenGL::ShaderUniformOpenGL() = default;

UniformObjectOpenGL ShaderUniformOpenGL::CreateUniformObject(GLenum type, int arrayCount) {
  if (arrayCount == 1) {
    switch (type) {
      case GL_FLOAT:
        return UniformObjectOpenGL(0.0f);
      case GL_INT:
        return UniformObjectOpenGL(0);
      case GL_FLOAT_VEC3:
        return UniformObjectOpenGL(Vector3());
      case GL_FLOAT_MAT4:
        return UniformObjectOpenGL(Matrix4x4());
      case GL_SAMPLER_2D:
        return UniformObjectOpenGL(0);
      default:
        throw "unsupported type";
    }
  } else {
    switch (type) {
      case GL_FLOAT_MAT4:
        return UniformObjectOpenGL(std::move(std::vector<Matrix4x4>(arrayCount, Matrix4x4())));
      case GL_SAMPLER_2D:
        return UniformObjectOpenGL(std::move(std::vector<int>(arrayCount, 0)));
      default:
        throw "unsupported type";
    }
  }
}

ShaderUniformOpenGL::ShaderUniformOpenGL(const ShaderUniformOpenGL& o) {
  _objectMap = o._objectMap;
}

ShaderUniformOpenGL::ShaderUniformOpenGL(ShaderUniformOpenGL&& o) {
  _objectMap = std::move(o._objectMap);
}

ShaderUniformOpenGL::~ShaderUniformOpenGL() = default;

ShaderUniformOpenGL& ShaderUniformOpenGL::operator=(const ShaderUniformOpenGL& o) {
  _objectMap = o._objectMap;
  return *this;
}

ShaderUniformOpenGL& ShaderUniformOpenGL::operator=(ShaderUniformOpenGL&& o) {
  _objectMap = std::move(o._objectMap);
  return *this;
}

ShaderUniformOpenGL::ShaderUniformOpenGL(const UniformDescMapOpenGL& map) {
  for (const auto& desc : map) {
    _objectMap.emplace(std::string_view(desc.first), CreateUniformObject(desc.second.type, desc.second.count));
  }
}

const UniformMapOpenGL& ShaderUniformOpenGL::GetUniformObjects() const { return _objectMap; }

std::shared_ptr<ShaderUniformOpenGL> Mine::CreateShaderUniformOpenGL(const ShaderProgramOpenGL& shader) {
  return std::make_shared<ShaderUniformOpenGL>(shader.GetUniformDesc());
}

MeshRendererOpenGL::MeshRendererOpenGL() = default;

MeshRendererOpenGL::MeshRendererOpenGL(const MeshRendererOpenGL& o) {
  shader = o.shader;
  material = o.material;
  mesh = o.mesh;
}

MeshRendererOpenGL::MeshRendererOpenGL(MeshRendererOpenGL&& o) {
  shader = std::move(o.shader);
  material = std::move(o.material);
  mesh = std::move(o.mesh);
}

MeshRendererOpenGL::~MeshRendererOpenGL() = default;

MeshRendererOpenGL& MeshRendererOpenGL::operator=(const MeshRendererOpenGL& o) {
  shader = o.shader;
  material = o.material;
  mesh = o.mesh;
  return *this;
}

MeshRendererOpenGL& MeshRendererOpenGL::operator=(MeshRendererOpenGL&& o) {
  shader = std::move(o.shader);
  material = std::move(o.material);
  mesh = std::move(o.mesh);
  return *this;
}

void MeshRendererOpenGL::Render() const {
  if (!shader.expired()) {
    auto s = shader.lock();
    s->Bind();
    if (!material.expired()) {
      auto m = material.lock();
      s->SetPass(m->GetUniformObjects());
    }
  }
  auto e = mesh.lock();
  e->Bind();
  MineGLFuncCall(glDrawElements(GL_TRIANGLES, e->GetIndexCount(), GL_UNSIGNED_INT, (void*)nullptr));
  MineGLFuncCall(glBindTexture(GL_TEXTURE_2D, 0));
}

GPUTexture2DOpenGL::GPUTexture2DOpenGL() : _handle(0), _width(0), _height(0) {}

GPUTexture2DOpenGL::GPUTexture2DOpenGL(const GPUTexture2DDescOpenGL& desc) {
  MineGLFuncCall(glGenTextures(1, &_handle));
  MineGLFuncCall(glBindTexture(GL_TEXTURE_2D, _handle));
  MineGLFuncCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, desc.wrapS));
  MineGLFuncCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, desc.wrapT));
  if (desc.wrapS == GL_CLAMP_TO_BORDER || desc.wrapT == GL_CLAMP_TO_BORDER) {
    MineGLFuncCall(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &desc.borderColor.x));
  }
  MineGLFuncCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, desc.minFliter));
  MineGLFuncCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, desc.magFliter));
  MineGLFuncCall(glTexImage2D(GL_TEXTURE_2D, 0, desc.format, desc.width, desc.height, 0, desc.dataFormat, desc.dataType, desc.dataPtr));
  if (desc.mipmapLevel > 0) {
    MineGLFuncCall(glGenerateMipmap(GL_TEXTURE_2D));
  }
  _width = desc.width;
  _height = desc.height;
}

GPUTexture2DOpenGL::GPUTexture2DOpenGL(GPUTexture2DOpenGL&& o) {
  _handle = o._handle;
  o._handle = 0;
}

GPUTexture2DOpenGL::~GPUTexture2DOpenGL() {
  Delete();
}

GPUTexture2DOpenGL& GPUTexture2DOpenGL::operator=(GPUTexture2DOpenGL&& o) {
  _handle = o._handle;
  o._handle = 0;
  return *this;
}

void GPUTexture2DOpenGL::Bind(GLenum id) const {
  MineGLFuncCall(glActiveTexture(id));
  MineGLFuncCall(glBindTexture(GL_TEXTURE_2D, _handle));
}

void GPUTexture2DOpenGL::Delete() {
  if (_handle != 0) {
    MineGLFuncCall(glDeleteTextures(1, &_handle));
  }
  _handle = 0;
}

std::shared_ptr<GPUTexture2DOpenGL> Mine::CreateTexture2DOpenGL(const GPUTexture2DDescOpenGL& desc) {
  return std::make_shared<GPUTexture2DOpenGL>(desc);
}

std::shared_ptr<GPUTexture2DOpenGL> Mine::CreateTexture2DOpenGL(const Texture2D& tex2d) {
  GPUTexture2DDescOpenGL desc;
  desc.wrapS = GL_REPEAT;
  desc.wrapT = GL_REPEAT;
  desc.minFliter = GL_NEAREST;
  desc.magFliter = GL_NEAREST;
  desc.mipmapLevel = 0;
  if (tex2d.GetChannels() == 4) {
    desc.format = GL_RGBA;
  } else {
    desc.format = GL_RGB;
  }
  desc.width = tex2d.GetWidth();
  desc.height = tex2d.GetHeight();
  if (tex2d.GetChannels() == 4) {
    desc.dataFormat = GL_RGBA;
  } else {
    desc.dataFormat = GL_RGB;
  }
  desc.dataType = GL_UNSIGNED_BYTE;
  desc.dataPtr = (GLvoid*)tex2d.GetData();
  return CreateTexture2DOpenGL(desc);
}

FrameBufferOpenGL::FrameBufferOpenGL() : _handle(0) {}

FrameBufferOpenGL::FrameBufferOpenGL(FrameBufferOpenGL&& o) {
  _handle = o._handle;
  o._handle = 0;
}

FrameBufferOpenGL& FrameBufferOpenGL::operator=(FrameBufferOpenGL&& o) {
  _handle = o._handle;
  o._handle = 0;
  return *this;
}

FrameBufferOpenGL::~FrameBufferOpenGL() {
  Delete();
}

void FrameBufferOpenGL::Bind() const {
  MineGLFuncCall(glBindFramebuffer(GL_FRAMEBUFFER, _handle));
}

void FrameBufferOpenGL::Unbind() const {
  MineGLFuncCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

bool FrameBufferOpenGL::BindTexture(const FrameBufferTextureDescOpenGL& desc) const {
  MineGLFuncCall(glFramebufferTexture(desc.target, desc.attachment, desc.texture, desc.level));
  GLenum result = MineGLFuncCall(glCheckFramebufferStatus(GL_FRAMEBUFFER));
  return result == GL_FRAMEBUFFER_COMPLETE;
}

void FrameBufferOpenGL::Delete() {
  if (_handle != 0) {
    MineGLFuncCall(glDeleteFramebuffers(1, &_handle));
  }
  _handle = 0;
}

std::shared_ptr<FrameBufferOpenGL> Mine::CreateFrameBufferOpenGL() {
  auto ptr = std::make_shared<FrameBufferOpenGL>();
  MineGLFuncCall(glGenFramebuffers(1, &(ptr->_handle)));
  return ptr;
}

ShadowMap2DOpenGL::ShadowMap2DOpenGL() = default;

ShadowMap2DOpenGL::ShadowMap2DOpenGL(int width, int height) {
  assert(width > 0 && height > 0);

  _frameBuffer = CreateFrameBufferOpenGL();

  GPUTexture2DDescOpenGL desc;
  desc.wrapS = GL_CLAMP_TO_EDGE;
  desc.wrapT = GL_CLAMP_TO_EDGE;
  desc.borderColor = Vector4(1, 1, 1, 1);
  desc.minFliter = GL_NEAREST;
  desc.magFliter = GL_NEAREST;
  desc.mipmapLevel = 0;
  desc.format = GL_DEPTH_COMPONENT;
  desc.width = width;
  desc.height = height;
  desc.dataFormat = GL_DEPTH_COMPONENT;
  desc.dataType = GL_FLOAT;
  desc.dataPtr = nullptr;
  _depthMap = CreateTexture2DOpenGL(desc);

  _frameBuffer->Bind();
  MineGLFuncCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthMap->GetHandle(), 0));
  GLenum result = MineGLFuncCall(glCheckFramebufferStatus(GL_FRAMEBUFFER));
  if (result != GL_FRAMEBUFFER_COMPLETE) {
    throw "cant init frame buffer";
  }
  MineGLFuncCall(glDrawBuffer(GL_NONE));
  MineGLFuncCall(glReadBuffer(GL_NONE));
  _frameBuffer->Unbind();
}

ShadowMap2DOpenGL::ShadowMap2DOpenGL(ShadowMap2DOpenGL&& o) {
  _frameBuffer = std::move(o._frameBuffer);
  _depthMap = std::move(o._depthMap);
}

ShadowMap2DOpenGL::~ShadowMap2DOpenGL() {
  Delete();
}

ShadowMap2DOpenGL& ShadowMap2DOpenGL::operator=(ShadowMap2DOpenGL&& o) {
  _frameBuffer = std::move(o._frameBuffer);
  _depthMap = std::move(o._depthMap);
  return *this;
}

void ShadowMap2DOpenGL::Bind() const {
  _frameBuffer->Bind();
}

void ShadowMap2DOpenGL::Unbind() const {
  _frameBuffer->Unbind();
}

void ShadowMap2DOpenGL::Delete() {
  if (_frameBuffer != nullptr) {
    _frameBuffer->Delete();
  }
  if (_depthMap != nullptr) {
    _depthMap->Delete();
  }
}

const GPUTexture2DOpenGL& ShadowMap2DOpenGL::GetDepthMap() const {
  return *_depthMap;
}

static std::string __head("light[");
static std::string __intensityTail("].intensity");
static std::string __posTail("].pos");
static std::string __colorTail("].color");

void Mine::SetPointLightValues(const PointLight& light, int index, ShaderUniformOpenGL& uniform) {
  uniform.SetValue(__head + std::to_string(index) + __intensityTail, light.intensity);
  uniform.SetValue(__head + std::to_string(index) + __posTail, light.pos);
  uniform.SetValue(__head + std::to_string(index) + __colorTail, light.color);
}