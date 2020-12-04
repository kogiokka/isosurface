#include "scene.h"
#include <iostream>

int
main()
{
  Scene scene;
  scene.Init();
  scene.Setup();
  scene.Render();

  return 0;
}
