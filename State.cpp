#include "State.h"

using namespace std;

void VehicleType::setType(const string& type)
{
  width = -1;
  height = -1;
  name = type;
  
  if (type == "StandardVehicle")
    {
      width = 1.22;
      height = 0.72;
    }
  else if (type == "LongVehicle")
    {
      width = 2.0;
      height = 0.72;
    }
  else if (type == "Motorcycle")
    {
      width = 0.9;
      height = 0.35;
    }
  else
    error("VehicleType::setType", "Invalid vehicle type: " + type);
  
  if (width < 0 || height < 0)
    error("VehicleType::setType", "Forgot to specify width/height for vehicle type: " + type + " ?");
}

State::State(const string& str)
{
    //qV expressed as fraction of MAX_SPEED defined in systemParms.h
    // desiredV = desired velocity/MAX_SPEED, not necessarily the maximum allowed
    // 
  double qX, qY, qTheta, qV;
  
  vector<string> tokens = split(str, " ");
  if (tokens.size() != 4)
    error("State::State(const string&)", "States should be 4 per vehicle, see configuration file");
  
  else
    {
      qX = stod(tokens[0]);
      qY = stod(tokens[1]);
      qTheta = stod(tokens[2]);
      qV = stod(tokens[3]);
    }
  
  init(qX, qY, qTheta, qV);
}

void State::init(const double& qX, const double& qY, const double& qTheta, const double& qV)
{
  /* error handling */
  if(qV > 1 || qV < 0)
    error("State::init", "v MUST be between 0 and 1");
  
  x = qX;
  y = qY;
  theta = qTheta;
  v = qV*MAX_SPEED;  
}

Vector<double, 2> State::toPoint() const
{
    Vector<double, 2> p;
    p[0] = x;
    p[1] = y;
    
    return p;
}

ostream& operator<<(ostream& os, const State& s)
{
    os << "State (x = " << fixed << setprecision(3) << s.x;
    os << ", y = " << s.y;
    os << ", theta = " << s.theta;
    os << ", v = " << s.v << ')';
    os.flush();
    
    return os;
}

ostream& operator<<(ostream& os, const Parms& s)
{
    os << "Parms (desiredV = " << fixed << setprecision(3) << s.desiredV;
    os << ", initManeuver = " << s.initManeuver;
    os << ", vehicleType = " << s.vehicleType.getName();
    os << ", pLayerType = " << s.pLayerType;
    os << ", ruleType = " << s.ruleType << ')';
    os.flush();
    
    return os;
}


bool operator==(const State& s1, const State& s2)
{
    return s1.x == s2.x && s1.y == s2.y &&
      s1.theta == s2.theta && s1.v == s2.v;
}

bool operator!=(const State& s1, const State& s2){
    return !(s1 == s2);
}

std::pair<State, Parms> State::makeStatesAndParms(const std::string& str)
{
  //qV expressed as fraction of MAX_SPEED defined in systemParms.h
  // desiredV = desired velocity/MAX_SPEED, not necessarily the maximum allowed
  // 
  double qX = -100, qY = -100, qTheta = -100, qV = -100, qDesiredV = -100;
  Maneuver qInitMan = UNKNOWN;
  VehicleType vType;
  string pType, rType; 
  
  vector<string> tokens = split(str, " ");
  if (tokens.size() < 4)
    error("State::State(const string&)", "States should be at least 4 per vehicle, see configuration file");
  
  if (tokens.size() == 4)
    {
      qX = stod(tokens[0]);
      qY = stod(tokens[1]);
      qTheta = stod(tokens[2]);
      qV = stod(tokens[3]);
      qDesiredV = 1; /* max velocity by default */
      qInitMan = FAST;
      vType.setType("StandardVehicle");
      pType = "StandardUnicycle";
      rType = "StandardRules";
    }
  else if (tokens.size() == 9)
    {
      qX = stod(tokens[0]);
      qY = stod(tokens[1]);
      qTheta = stod(tokens[2]);
      qV = stod(tokens[3]);
      qDesiredV = stod(tokens[4]);
      qInitMan = strToManeuver(tokens[5]);
      vType.setType(tokens[6]);
      pType = tokens[7];
      rType = tokens[8];
    }
  else
    error("State::State(const string&)", "Too few arguments");

  State q(qX, qY, qTheta, qV);
  Parms p(qDesiredV, qInitMan, vType, pType, rType);

  return std::make_pair(q, p);
  
}
