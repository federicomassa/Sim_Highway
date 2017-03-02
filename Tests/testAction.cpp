#include "../LeftAction.h"
#include "../ActionManager.h"
#include <iostream>
#include <fstream>

const Configuration CONF("Input/Simulator.conf");
int now = 0;

int main(int argc, char** argv)
{
  ActionManager aMan;
  aMan.init();

  std::ifstream in("simulatorLeft.dat");

  while (!in.eof())
    {
      aMan.parseTestFile(in);

      aMan.run();
      now++;
    }
  
  aMan.printHistory();


  
  return 0;
}
