#pragma once

#include <vector>

#include <OpenGLContext.h>
#include <Camera.h>

namespace Mine {

class ShadowPipeline;

struct BlinnPhongMaterial {
  Vector3 ka;
  Vector3 kd;
  Vector3 ks;
  float shininess;
  std::weak_ptr<GPUTexture2DOpenGL> diffuseTex;
  constexpr BlinnPhongMaterial() : ka(Vector3(0.05f, 0.05f, 0.05f)),
                                   kd(Vector3(1, 1, 1)),
                                   ks(Vector3(1, 1, 1)),
                                   shininess(64),
                                   diffuseTex() {}
  void SetValues(ShadowPipeline& pipeline, ShaderUniformOpenGL& uniform) const;
};

class Light {
 public:
  PointLight light;
  std::shared_ptr<ShaderUniformOpenGL> material;
  bool hasShadow;
  ShadowMap2DOpenGL shadowMap;
  Matrix4x4 lightSpaceVP;

  void SetValues(ShadowPipeline& pipeline, int index, int texSlot, ShaderUniformOpenGL& uniform) const;
};

class GameObject {
 public:
  std::weak_ptr<GPUMeshOpenGL> meshPtr;
  std::weak_ptr<ShaderProgramOpenGL> shader;
  std::shared_ptr<ShaderUniformOpenGL> material;
  BlinnPhongMaterial materialData;
  Vector3 pos;
  Vector3 scale;
};

class ShadowPipeline {
 private:
  std::shared_ptr<GPUMeshOpenGL> _lightCube;
  std::shared_ptr<ShaderProgramOpenGL> _lightCubeShader;
  std::shared_ptr<ShaderProgramOpenGL> _shadowShader;
  std::shared_ptr<ShaderUniformOpenGL> _shadowShaderUniform;

  std::vector<Light> _lights;
  std::vector<GameObject> _objects;

 public:
  int shadowWidth;
  int shadowHeight;
  Camera mainCamera;

  void Init();
  void Terminate();

  void AddLight(const PointLight& light, bool hasShadow);
  void AddObject(const std::shared_ptr<GPUMeshOpenGL>& ptr,
                 const std::shared_ptr<Mine::ShaderProgramOpenGL>& shader,
                 const BlinnPhongMaterial& blinn,
                 const Vector3& pos,
                 const Vector3& scale);
  void Render();
  std::vector<Light>& GetLights();
};

}  // namespace Mine
