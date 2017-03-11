#include "../ActionManager.h"
#include "../RuleMonitor.h"
#include "../Logger.h"
#include "../Area.h"
#include "../TestRules.h"
#include "../State.h"
#include "../Vector.h"
#include "../List.h"
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>

const Configuration CONF("Input/Simulator.conf");
int now = 0;
Logger LOG("testAction.log");

Vector<State, 10> monitorStates;
Vector<List<State>, 10> neighStates;

void parseTestFile(std::ifstream& input)
{
  std::string line;

  if (!input.eof())
    {
      double x, y, theta, v;
      
      getline(input, line, ' ');
      x = atof(line.c_str());

      getline(input, line, ' ');
      y = atof(line.c_str());

      getline(input, line, ' ');
      theta = atof(line.c_str());

      getline(input, line, '\n');
      v = atof(line.c_str());
      
      State q(x, y, theta, v, 0);

      /*      std::cout << q << std::endl;*/
      
      monitorStates.insHead(q);
      List<State> neigh;
      neighStates.insHead(neigh); /* Empty for testing. Only monitor in the env */
    }
} 


int main(int argc, char** argv)
{
  ActionManager aMan(monitorStates, neighStates);
  aMan.init();
  
  RuleMonitor rMon(aMan);
  rMon.setRules(new TestRules);

  std::ifstream in("simulatorLeft.dat");

  parseTestFile(in);
  parseTestFile(in);
  parseTestFile(in);
  now = now + 3;
  
  while (!in.eof())
    {
      Area obs;

      /* Testing: left lane is visible */
      Vector<Vector<double,2>, 2> bounds;
      bounds[0][0] = -100.0;
      bounds[0][1] = 100.0;
      bounds[1][0] = 1.0;
      bounds[1][1] = 2.0;
      obs.addRect(bounds);


      
      parseTestFile(in);
      aMan.run();
      rMon.run(obs);
      now++;
      std::cout << now << std::endl;
      aMan.printHistory();
      std::cout << "~~~~~~~~~~~~" << std::endl;
    }
  



  
  return 0;
}
