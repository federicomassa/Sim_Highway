#include "../ActionManager.h"
#include "../RuleMonitor.h"
#include "../Logger.h"
#include "../Area.h"
#include "../TestRules.h"
#include "../State.h"
#include "../Sensing.h"
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
Vector<List<Sensing>, 10> neighStates;

void parseTestFile(std::ifstream& input1, std::ifstream& input2)
{
  std::string line1;
  std::string line2;

  if (!input1.eof() || !input2.eof())
    {
      double x, y, theta, v;
      double x2, y2, theta2, v2;
      
      getline(input1, line1, ' ');
      getline(input2, line2, ' ');
      x = atof(line1.c_str());
      x2 = atof(line2.c_str());

      getline(input1, line1, ' ');
      getline(input2, line2, ' ');
      y = atof(line1.c_str());
      y2 = atof(line2.c_str());

      getline(input1, line1, ' ');
      getline(input2, line2, ' ');
      theta = atof(line1.c_str());
      theta2 = atof(line2.c_str());

      getline(input1, line1, '\n');
      getline(input2, line2, '\n');
      v = atof(line1.c_str());
      v2 = atof(line2.c_str());
      
      State q(x, y, theta, v, 0);
      State q2(x2, y2, theta2, v2, 0);
      Sensing s(1, q2, q2.v, UNKNOWN);
      /*      std::cout << q << std::endl;*/
      
      monitorStates.insHead(q);
      List<Sensing> neigh;
      neigh.insHead(s);
      neighStates.insHead(neigh);
    }
} 


int main(int argc, char** argv)
{
  ActionManager aMan(monitorStates, neighStates);
  aMan.init();
  
  RuleMonitor rMon(aMan);
  rMon.setRules(new TestRules);

  std::ifstream in("simulatorLeft.dat");
  std::ifstream in2("simulatorOther.dat");

  parseTestFile(in, in2);
  parseTestFile(in, in2);
  parseTestFile(in, in2);
  now = now + 3;
  
  while (!in.eof())
    {
      Area obs;

      /* Testing: left lane is visible */
      Vector<Vector<double,2>, 2> bounds;
      bounds[0][0] = -100.0;
      bounds[0][1] = 100.0;
      bounds[1][0] = 0.0;
      bounds[1][1] = 2.0;
      obs.addRect(bounds);


      
      parseTestFile(in, in2);
      aMan.run();
      rMon.run(obs);
      now++;
      std::cout << "Time: " << now << std::endl;
      aMan.printHistory();
      std::cout << "~~~~~~~~~~~~" << std::endl;
    }
  



  
  return 0;
}
