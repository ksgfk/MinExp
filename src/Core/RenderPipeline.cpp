#include "RenderPipeline.h"

using namespace Mine;

MeshRenderer::MeshRenderer(GPUMeshOpenGL&& m) {
  _mesh = std::move(m);
}