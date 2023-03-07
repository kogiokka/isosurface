#include "MainWindow.hpp"
#include <iostream>

int
main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
  MainWindow app("Isosurface", 800, 800);
  app.Show();

  return 0;
}
