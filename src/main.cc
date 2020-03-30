#include "isosurface.h"
#include "scene.h"
#include <iostream>

int
main(int argc, char** argv)
{
  Isosurface isosurface;
  isosurface.ReadRawInfo("data/engine.inf");
  isosurface.ReadRaw("data/engine.raw");
  isosurface.CalculateGradient();
  isosurface.MarchingCube();

  Scene scene;
  scene.Init();
  scene.SetupOpenGL(isosurface.RenderVertexCount(), isosurface.RenderData());
  scene.Render();

  return 0;
}
