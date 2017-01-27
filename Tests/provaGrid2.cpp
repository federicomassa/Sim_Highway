#include "../Grid.h"

int main()
{
  Grid g(900, 900, 0.1, 20, 20);
  g.drawGrid();
  g.drawAxes();
  g.drawLabels("prova x", 0, 10, "prova y", 100, 110);
  g.save("Ciao", "1");
  
  return 0;
}
