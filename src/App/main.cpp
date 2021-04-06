#include <chrono>

#include <OpenGLContext.h>
#include <Camera.h>
#include <Input.h>
#include <iostream>

#include "ShadowPipeline.h"

Mine::Camera cam;
Mine::OrbitMotion orbit;
Mine::PointLight light;
Mine::ShadowPipeline pipeline;

std::shared_ptr<Mine::GPUMeshOpenGL> planeBuffer;
std::shared_ptr<Mine::GPUMeshOpenGL> cubeBuffer;
std::shared_ptr<Mine::GPUTexture2DOpenGL> cubeTexBuffer;

//ying
std::vector<std::shared_ptr<Mine::GPUMeshOpenGL>> yingBuffer;
std::vector<std::shared_ptr<Mine::GPUTexture2DOpenGL>> yingTexBuffer;

void loadYing() {
  Mine::MultiMesh ying;
  std::vector<Mine::Texture2D> yingTex;
  ying = Mine::LoadObjWithChildFromFile(std::filesystem::current_path() / "asset" / "ying" / "ying");
  yingTex.emplace_back(Mine::Texture2D(std::filesystem::current_path() / "asset" / "ying" / "hair.png"));
  yingTex.emplace_back(Mine::Texture2D(std::filesystem::current_path() / "asset" / "ying" / "face.png"));
  yingTex.emplace_back(Mine::Texture2D(std::filesystem::current_path() / "asset" / "ying" / "expression.png"));
  yingTex.emplace_back(Mine::Texture2D(std::filesystem::current_path() / "asset" / "ying" / "cloth.png"));
  Mine::Mesh me;
  me.attrib = std::move(ying.attrib);
  for (auto& m : ying.obj) {
    me.face = m.second;
    yingBuffer.emplace_back(std::move(Mine::CreateMeshBufferOpenGL(me)));
  }
  for (const auto& t : yingTex) {
    yingTexBuffer.emplace_back(std::move(Mine::CreateTexture2DOpenGL(t)));
  }
}

void destroyYing() {
  for (auto& t : yingBuffer) {
    t->Delete();
  }
  for (auto& t : yingTexBuffer) {
    t->Delete();
  }
}

void loadGrassCube() {
  Mine::Mesh cube;
  Mine::Mesh plane;
  Mine::Texture2D cubeTex2d;
  cube = Mine::LoadObjFromFile(std::filesystem::current_path() / "asset" / "cube");
  cubeTex2d = Mine::Texture2D(std::filesystem::current_path() / "asset" / "cube.png");
  cubeBuffer = Mine::CreateMeshBufferOpenGL(cube);
  cubeTexBuffer = Mine::CreateTexture2DOpenGL(cubeTex2d);
  plane = Mine::LoadObjFromFile(std::filesystem::current_path() / "asset" / "plane");
  planeBuffer = Mine::CreateMeshBufferOpenGL(plane);
}

std::shared_ptr<Mine::ShaderProgramOpenGL> unlit;
void loadBlinnPhongShader() {
  unlit = Mine::CreateShaderProgramOpenGL(std::filesystem::current_path() / "asset" / "blinn_phong");
}

void clear() {
  unlit->Delete();
  cubeBuffer->Delete();
  cubeTexBuffer->Delete();
  planeBuffer->Delete();
  destroyYing();
}

void setupPipeline() {
  Mine::PointLight l;
  l.intensity = 0.5f;
  l.pos = Mine::Vector3(3.0f, 3.0f, 0);
  l.color = Mine::Vector3(1, 1, 1);
  pipeline.AddLight(l, true);

  l.pos = Mine::Vector3(-4, 3, -4);
  l.intensity = 3;
  l.color = Mine::Vector3(1, 0, 0);
  pipeline.AddLight(l, true);

  l.pos = Mine::Vector3(4, 6, -4);
  l.intensity = 1;
  l.color = Mine::Vector3(0, 1, 0);
  pipeline.AddLight(l, true);

  l.pos = Mine::Vector3(4, 6, 4);
  l.intensity = 1;
  l.color = Mine::Vector3(0, 0, 1);
  pipeline.AddLight(l, true);

  Mine::BlinnPhongMaterial b;
  // b.ka = Mine::Vector3(0.01f, 0.01f, 0.01f);
  // b.kd = Mine::Vector3(1.0f, 1.0f, 1.0f);
  // b.ks = Mine::Vector3(1.0f, 1.0f, 1.0f);
  // b.shininess = 128;
  // b.diffuseTex = cubeTexBuffer;
  // pipeline.AddObject(cubeBuffer, unlit, b, Mine::Vector3(0, 1, 0), Mine::Vector3(1, 1, 1));

  b.ka = Mine::Vector3(0.01f, 0.01f, 0.01f);
  b.kd = Mine::Vector3(1.0f, 1.0f, 1.0f);
  b.ks = Mine::Vector3(0.5f, 0.5f, 0.5f);
  b.shininess = 2;
  b.diffuseTex = std::weak_ptr<Mine::GPUTexture2DOpenGL>();
  pipeline.AddObject(planeBuffer, unlit, b, Mine::Vector3(0, 0, 0), Mine::Vector3(1, 1, 1));

  for (int i = 0; i < 4; i++) {
    b.diffuseTex = std::weak_ptr<Mine::GPUTexture2DOpenGL>(yingTexBuffer[i]);
    pipeline.AddObject(yingBuffer[i], unlit, b, Mine::Vector3(0, 0, 0), Mine::Vector3(3, 3, 3));
  }
}

long long deltaTime;
long long allTime;

int main() {
  Mine::InitOpenGL(1280, 720, "test");
  loadGrassCube();
  loadYing();
  loadBlinnPhongShader();
  pipeline.shadowWidth = 2048;
  pipeline.shadowHeight = 2048;
  pipeline.Init();
  setupPipeline();

  do {
    auto start = std::chrono::steady_clock::now();
    auto [fbw, fbh] = Mine::GetFrameBufferSizeOpenGL();
    Mine::Input::GetInstance().UpdateState();
    orbit.UpdateData(fbh);
    orbit.UpdateCamera(pipeline.mainCamera);

    pipeline.Render();

    auto end = std::chrono::steady_clock::now();
    auto delta = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    deltaTime = delta.count();
    allTime += deltaTime;
    for (auto& l : pipeline.GetLights()) {
      auto x = std::sin(allTime * 0.00005f) * 0.02f;
      auto y = std::cos(allTime * 0.00005f) * 0.03f;
      auto z = std::cos(allTime * 0.00005f) * 0.02f;
      l.light.pos = Mine::Add(l.light.pos, Mine::Vector3(x, y, z));
    }
  } while (!Mine::ShouldTerminateOpenGL());

  pipeline.Terminate();
  clear();
  Mine::TerminateOpenGL();
  return 0;
}