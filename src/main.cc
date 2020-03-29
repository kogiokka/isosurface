#include "isosurface.h"
#include "scene.h"

int
main(int argc, char** argv)
{
  Isosurface isosurface;
  isosurface.ReadRawInfo("data/engine.inf");
  isosurface.ReadRaw("data/engine.raw");

  // Scene scene;
  // scene.Init();
  // scene.SetupOpenGL();
  // scene.Render();

  return 0;
}
