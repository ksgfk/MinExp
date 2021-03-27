#pragma once

#include <vector>

#include "Camera.h"
#include "OpenGLContext.h"

namespace Mine {

class MeshRenderer {
 private:
  GPUMeshOpenGL _mesh;

 public:
  MeshRenderer(GPUMeshOpenGL&& m);
  void Render();
};

struct RenderContext {
};

class RenderPipeline {
 public:
  void Render(const RenderContext& ctx, const Camera& camera);
};

}  // namespace Mine
