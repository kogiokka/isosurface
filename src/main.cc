#include "scene.h"
#include <iostream>

int
main(int argc, char** argv)
{
  Scene scene;
  scene.Init();
  scene.Setup();
  scene.Render();

  return 0;
}
