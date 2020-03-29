#include "isosurface.h"
#include "scene.h"

int
main(int argc, char** argv)
{
  Scene scene;
  scene.Init();
  scene.SetupOpenGL();
  scene.Render();

  return 0;
}
