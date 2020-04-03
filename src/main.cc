#include "isosurface.h"
#include "model.h"
#include "scene.h"
#include <iostream>

int
main(int argc, char** argv)
{
  Model engine;
  Isosurface iso_model(80.f);
  iso_model.ReadInfo("data/engine.inf");
  iso_model.ReadRaw("data/engine.raw");
  iso_model.CalculateGradient();
  engine.SetRenderData(iso_model.MarchingCube());

  Scene scene;
  scene.Init();
  scene.SetupOpenGL(engine.VertexCount(), engine.RenderData());
  scene.Render();

  return 0;
}
