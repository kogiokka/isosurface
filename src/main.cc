#include "isosurface.h"
#include "model.h"
#include "scene.h"
#include <iostream>

int
main(int argc, char** argv)
{
  Model engine("data/foot.inf", "data/foot.raw");
  Isosurface isosurface(engine.ScalarField());
  isosurface.SetIsovalue(95.f);
  isosurface.SetModelDimensions(engine.Dimension());
  isosurface.SetModelRatio(engine.Ratio());

  engine.SetRenderData(isosurface.MarchingCube());

  Scene scene;
  scene.Init();
  scene.SetPosition(engine.Center());
  scene.SetupOpenGL(engine.VertexCount(), engine.RenderData());
  scene.Render();

  return 0;
}
