#include "ShadowPipeline.h"

using namespace Mine;

void BlinnPhongMaterial::SetValues(ShadowPipeline& pipeline, ShaderUniformOpenGL& uniform) const {
  uniform.SetValue("ka", ka);
  uniform.SetValue("kd", kd);
  uniform.SetValue("ks", ks);
  uniform.SetValue("shininess", shininess);
  if (diffuseTex.expired()) {
    MineGLFuncCall(glActiveTexture(GL_TEXTURE0));
    MineGLFuncCall(glBindTexture(GL_TEXTURE_2D, 0));
    uniform.SetValue("diffuseTex", 0);
  } else {
    auto ptr = diffuseTex.lock();
    ptr->Bind(GL_TEXTURE0);
    uniform.SetValue("diffuseTex", 0);
  }
}

static std::string __head("light[");
static std::string __shadowMapTail("].shadowMap");

void Light::SetValues(ShadowPipeline& pipeline, int index, int texSlot, ShaderUniformOpenGL& uniform) const {
  if (hasShadow) {
    shadowMap.GetDepthMap().Bind(GL_TEXTURE0 + texSlot);
    uniform.SetValue(__head + std::to_string(index) + __shadowMapTail, texSlot);
  } else {
    MineGLFuncCall(glActiveTexture(GL_TEXTURE0 + texSlot));
    MineGLFuncCall(glBindTexture(GL_TEXTURE_2D, 0));
  }
}

void ShadowPipeline::Init() {
  auto cube = Mine::LoadObjFromFile(std::filesystem::current_path() / "asset" / "cube");
  _lightCube = Mine::CreateMeshBufferOpenGL(cube, false, false);
  _lightCubeShader = Mine::CreateShaderProgramOpenGL(std::filesystem::current_path() / "asset" / "light");
  _shadowShader = Mine::CreateShaderProgramOpenGL(std::filesystem::current_path() / "asset" / "shadow");
  _shadowShaderUniform = Mine::CreateShaderUniformOpenGL(*_shadowShader);
}

void ShadowPipeline::Terminate() {
  _lightCube->Delete();
  _lightCubeShader->Delete();
  _shadowShader->Delete();
  for (auto& l : _lights) {
    l.shadowMap.Delete();
  }
}

void ShadowPipeline::AddLight(const PointLight& light, bool hasShadow) {
  Light l;
  l.hasShadow = hasShadow;
  l.light = light;
  l.material = Mine::CreateShaderUniformOpenGL(*_lightCubeShader);
  if (l.hasShadow) {
    l.shadowMap = ShadowMap2DOpenGL(shadowWidth, shadowHeight);
  }
  _lights.emplace_back(std::move(l));
}

void ShadowPipeline::AddObject(const std::shared_ptr<GPUMeshOpenGL>& ptr,
                               const std::shared_ptr<Mine::ShaderProgramOpenGL>& shader,
                               const BlinnPhongMaterial& blinn,
                               const Vector3& pos,
                               const Vector3& scale) {
  GameObject go;
  go.meshPtr = std::weak_ptr<GPUMeshOpenGL>(ptr);
  go.shader = shader;
  go.material = Mine::CreateShaderUniformOpenGL(*shader);
  go.pos = pos;
  go.scale = scale;
  go.materialData = blinn;
  _objects.emplace_back(std::move(go));
}

void ShadowPipeline::Render() {
  MeshRendererOpenGL mr;
  auto [fbw, fbh] = Mine::GetFrameBufferSizeOpenGL();

  //shadow pass
  for (auto& light : _lights) {
    if (light.hasShadow) {
      light.shadowMap.Bind();
      mr.shader = _shadowShader;
      MineGLFuncCall(glClearColor(0, 0, 0, 1));
      MineGLFuncCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
      MineGLFuncCall(glViewport(0, 0, shadowWidth, shadowWidth));
      MineGLFuncCall(glEnable(GL_DEPTH_TEST));
      MineGLFuncCall(glDisable(GL_CULL_FACE));
      auto&& look = Mine::LookAtRH(light.light.pos, Vector3(0, 0, 0), Vector3(0, 1, 0));
      auto&& ortho = Mine::OrthoRH(-15, 15, -15, 15, 0.1f, 30);
      light.lightSpaceVP = Mul(ortho, look);
      for (const auto& go : _objects) {
        auto&& model = Scale(Translation(go.pos), go.scale);
        auto&& mvp = Mul(light.lightSpaceVP, model);
        _shadowShaderUniform->SetValue("lightMVP", mvp);
        mr.material = _shadowShaderUniform;
        mr.mesh = go.meshPtr;
        mr.Render();
      }
      light.shadowMap.Unbind();
    }
  }
  MineGLFuncCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

  //normal pass
  MineGLFuncCall(glClearColor(0, 0, 0, 1));
  MineGLFuncCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
  MineGLFuncCall(glViewport(0, 0, fbw, fbh));
  MineGLFuncCall(glEnable(GL_DEPTH_TEST));
  MineGLFuncCall(glEnable(GL_CULL_FACE));
  auto&& view = mainCamera.View();
  auto&& proj = mainCamera.Projection();
  auto&& vp = Mul(proj, view);
  mr.mesh = _lightCube;
  mr.shader = _lightCubeShader;
  for (const auto& light : _lights) {
    auto&& model = Scale(Translation(light.light.pos), Vector3(0.01f, 0.01f, 0.01f));
    auto&& mvp = Mul(vp, model);
    light.material->SetValue("mvp", mvp);
    light.material->SetValue("color", light.light.color);
    mr.material = light.material;
    mr.Render();
  }

  for (const auto& go : _objects) {
    auto&& model = Scale(Translation(go.pos), go.scale);
    auto&& mvp = Mul(vp, model);

    go.material->SetValue("mvp", mvp);
    go.material->SetValue("lightCount", (int)_lights.size());
    go.material->SetValue("eyePos", mainCamera.pos);
    go.materialData.SetValues(*this, *go.material);
    for (int i = 0; i < _lights.size(); i++) {
      go.material->SetArray("lightMVP", i, Mul(_lights[i].lightSpaceVP, model));
      Mine::SetPointLightValues(_lights[i].light, i, *go.material);
      _lights[i].SetValues(*this, i, i + 1, *go.material);  //hard core shadow map slot
    }

    mr.material = go.material;
    mr.mesh = go.meshPtr;
    mr.shader = go.shader;
    mr.Render();
  }
}

std::vector<Light>& ShadowPipeline::GetLights() {
  return _lights;
}