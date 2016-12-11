#include "State.h"

State::State(const string& str)
{
    stringstream token(str);

    //qV expressed as fraction of MAX_SPEED defined in systemParms.h
    // desiredV = desired velocity/MAX_SPEED, not necessarily the maximum allowed
    // 
    double qX, qY, qTheta, qV, qDesiredV;
    string qInitManeuver; 

    token >> qX;
    token >> qY;
    token >> qTheta;
    token >> qV;

    // Added by Federico Massa --- mainly for platoon
    token >> qDesiredV;
    token >> qInitManeuver;

    init(qX, qY, qTheta, qV, qDesiredV, qInitManeuver);
}

void State::init(double qX, double qY, double qTheta, double qV, double qDesiredV, string qInitManeuver)
{
    /* error handling */
  if(qV > 1 || qV < 0 || qDesiredV > 1 || qDesiredV < 0)
    error("State::init", "v MUST be between 0 and 1");
    
  x = qX;
  y = qY;
  theta = qTheta;
  v = qV*MAX_SPEED;

  // Mainly for platoon --- Added by Federico Massa
  desiredV = qDesiredV*MAX_SPEED;
  initManeuver = qInitManeuver;

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

bool operator==(const State& s1, const State& s2)
{
    return s1.x == s2.x && s1.y == s2.y &&
           s1.theta == s2.theta && s1.v == s2.v;
}

bool operator!=(const State& s1, const State& s2){
    return !(s1 == s2);
}
