#include "isosurface.h"
#include "model.h"
#include "scene.h"
#include <iostream>

int
main(int argc, char** argv)
{
  Model model("data/engine.inf", "data/engine.raw");
  Isosurface isosurface(model.ScalarField());
  isosurface.SetIsovalue(80.f);
  isosurface.SetModelDimensions(model.Dimension());
  isosurface.SetModelRatio(model.Ratio());

  model.SetRenderData(isosurface.MarchingCube());

  Scene scene;
  scene.Init();
  scene.SetPosition(model.Center());
  scene.SetupOpenGL(model.VertexCount(), model.RenderData());
  scene.Render();

  return 0;
}
