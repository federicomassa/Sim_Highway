#ifndef STATE_H
#define STATE_H

#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>

#include "systemParms.h"
#include "systemTypes.h"
#include "utility.h"
#include "Vector.h"

class VehicleType
{
  double width;
  double height;
  std::string name;
 public:
  VehicleType() {width = -1; height = -1;}
  friend int size(const VehicleType& vT) {return size(vT.width) + size(vT.height) + size(vT.name);}
  void setType(const std::string&);
  const double& getWidth() const {return width;}
  const double& getHeight() const {return height;}
  const std::string& getName() const {return name;}
  bool operator==(const VehicleType& vT) const
  {
    return (width == vT.width) && (height == vT.height) && (name == vT.name);
  }
  
};


struct Parms
{
  double desiredV;
  Maneuver initManeuver;
  VehicleType vehicleType;
  std::string pLayerType;
  std::string ruleType;

  Parms(const double& qDesiredV,
	const Maneuver& qInitManeuver,
	const VehicleType& vType,
	const std::string& pType,
	const std::string& rType) : desiredV(qDesiredV), initManeuver(qInitManeuver)
  {
    vehicleType = vType;
    pLayerType = pType;
    ruleType = rType;
  }

  Parms()
  {
    desiredV = MAX_SPEED;
    initManeuver = FAST;
    vehicleType.setType("StandardVehicle");
    pLayerType = "StandardUnicycle";
    ruleType = "StandardRules";    
  }

  bool operator==(const Parms& p) const
  {
    return ((this->desiredV == p.desiredV) &&
	    (this->initManeuver == p.initManeuver) &&
	    (this->vehicleType == p.vehicleType) &&
	    (this->pLayerType == p.pLayerType) &&
	    (this->ruleType == p.ruleType));
	    
  }
  
};

class State
{
    friend int size(State s)
    {
        return size(s.x) + size(s.y) + size(s.theta) + size(s.v);
    }


    
    /* initialize state's members */
    void init(const double& qX, const double& qY, const double& qTheta, const double& qV);
 public:
    double x;
    double y;
    double theta;
    double v;
    
    /* constructor */
    State(const double& qX = 0, const double& qY = 0, const double& qTheta = 0, const double& qV = 0)
      {
	init(qX, qY, qTheta, qV);
      }
    State(const std::string& str);
    /* destructor */
    ~State() { }
    /* calculate the distance from another state */
    double distanceFrom(const State& s) const
    {
      return sqrt(pow(x - s.x, 2) + pow(y - s.y, 2));
    }
    /* conversion from State to double[Q_DIM] */
    Vector<double, 2> toPoint() const;

    static std::pair<State, Parms> makeStatesAndParms(const std::string&);
};

std::ostream& operator<<(std::ostream&, const State&);
std::ostream& operator<<(std::ostream&, const Parms&);
bool operator==(const State&, const State&);
bool operator!=(const State&, const State&);

#endif
