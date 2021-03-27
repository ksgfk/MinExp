#include <OpenGLContext.h>
#include <Camera.h>
#include <Input.h>
#include <iostream>

std::shared_ptr<Mine::ShaderProgramOpenGL> unlit;
std::shared_ptr<Mine::ShaderUniformOpenGL> unlitMat;
std::shared_ptr<Mine::GPUMeshOpenGL> cubeBuffer;
std::shared_ptr<Mine::GPUTexture2DOpenGL> cubeTexBuffer;
Mine::MeshRendererOpenGL mr;
Mine::Mesh cube;
Mine::Texture2D cubeTex2d;
Mine::Camera cam;
Mine::OrbitMotion orbit;
Mine::PointLight light;

int main() {
  Mine::InitOpenGL(1280, 720, "test");
  cube = Mine::LoadObjFromFile(std::filesystem::current_path() / "asset" / "cube");
  cubeTex2d = Mine::Texture2D(std::filesystem::current_path() / "asset" / "cube.png");
  unlit = Mine::CreateShaderProgramOpenGL(std::filesystem::current_path() / "asset" / "unlit");
  unlitMat = Mine::CreateShaderUniformOpenGL(*unlit);
  cubeBuffer = Mine::CreateMeshBufferOpenGL(cube);
  mr.mesh = cubeBuffer;
  cubeTexBuffer = Mine::CreateTexture2DOpenGL(cubeTex2d);

  do {
    auto [fbw, fbh] = Mine::GetFrameBufferSizeOpenGL();
    Mine::Input::GetInstance().UpdateState();
    orbit.UpdateData(fbh);
    orbit.UpdateCamera(cam);

    MineGLFuncCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    MineGLFuncCall(glClearColor(0, 0, 0, 1));
    MineGLFuncCall(glViewport(0, 0, fbw, fbh));
    MineGLFuncCall(glEnable(GL_DEPTH_TEST));
    cam.aspect = (float)fbw / fbh;
    auto&& mvp = Mul(Mul(cam.Projection(), cam.View()), Mine::Matrix4x4::Identity());
    unlitMat->SetValue("mvp", mvp);
    Mine::SetPointLightOpenGL(*unlitMat, light, cam.pos);
    unlit->SetPass(unlitMat->GetUniformObjects());
    cubeTexBuffer->Bind(GL_TEXTURE0);
    mr.Render();
  } while (!Mine::ShouldTerminateOpenGL());

  unlit->Delete();
  cubeBuffer->Delete();
  cubeTexBuffer->Delete();

  Mine::TerminateOpenGL();
  return 0;
}