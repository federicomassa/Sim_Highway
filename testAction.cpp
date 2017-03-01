#include "LeftAction.h"
#include "ActionManager.h"
#include <iostream>

int now = 0;

int main(int argc, char** argv)
{
  ActionManager aMan;
  aMan.init();

  aMan.run();
  now++;
  aMan.run();
  now++;
  aMan.run();
  now++;
  aMan.run();
  now++;


  aMan.printHistory();
  
  return 0;
}
