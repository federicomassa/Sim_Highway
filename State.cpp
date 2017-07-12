#include "State.h"

using namespace std;

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

  init(qX, qY, qTheta, qV * MAX_SPEED, qDesiredV * MAX_SPEED, qInitManeuver);
}

State::State(const State& q)
{
  x = q.x;
  y = q.y;
  theta = q.theta;
  v = q.v;
  desiredV = q.desiredV;
  initManeuver = q.initManeuver;
}

State::State(double qX, double qY, double qTheta, double qV, double qDesiredV, string qInitManeuver)
{
  x = qX;
  y = qY;
  theta = qTheta;
  v = qV;

  // Mainly for platoon --- Added by Federico Massa
  desiredV = qDesiredV;
  initManeuver = qInitManeuver;

}
void State::init(double qX, double qY, double qTheta, double qV, double qDesiredV, string qInitManeuver)
{
  /* error handling */
  //if (qV > MAX_SPEED || qV < 0 || qDesiredV > 1 || qDesiredV < 0)
  //  error("State::init", "v MUST be between 0 and 1");

  x = qX;
  y = qY;
  theta = qTheta;
  v = qV;

  // Mainly for platoon --- Added by Federico Massa
  desiredV = qDesiredV;
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
  os << "State (x = " << fixed << setprecision(4) << s.x;
  os << ", y = " << s.y;
  os << ", theta = " << s.theta;
  os << ", v = " << s.v;
  os << ", desiredV = " << s.desiredV << ')';
  os.flush();

  return os;
}

bool operator==(const State& s1, const State& s2)
{
  return s1.x == s2.x && s1.y == s2.y &&
         s1.theta == s2.theta && s1.v == s2.v &&
         s1.desiredV == s2.desiredV &&
         s1.initManeuver == s2.initManeuver;
}

bool operator!=(const State& s1, const State& s2) {
  return !(s1 == s2);
}

State operator+(const State& q1, const State& q2)
{
  if (q1.desiredV != q2.desiredV || q1.initManeuver != q2.initManeuver)
    error("State::operator+", "Cannot sum two state with different desireV or initManeuver");

  return State(q1.x + q2.x, q1.y + q2.y, q1.theta + q2.theta, q1.v + q2.v, q1.desiredV, q1.initManeuver);
}

State operator-(const State& q1, const State& q2)
{
  if (q1.desiredV != q2.desiredV || q1.initManeuver != q2.initManeuver)
  {
    error("State::operator-", "Cannot subtract two state with different desireV or initManeuver");
  }

  return State(q1.x - q2.x, q1.y - q2.y, q1.theta - q2.theta, q1.v - q2.v, q1.desiredV, q1.initManeuver);
}

State operator/(const State& q, const double& k)
{
  if (fabs(k) < 1E-10)
    error("State::operator/", "Cannot divide for zero");

  return State(q.x / k, q.y / k, q.theta / k, q.v / k, q.desiredV, q.initManeuver);
}


double& State::operator[](const int& i)
{
  if (i < 0 || i > 4)
    error("State::operator[]", "Index out of bounds");

  double* var = 0;

  switch (i)
  {
  case 0:
    var = &x;
    break;
  case 1:
    var = &y;
    break;
  case 2:
    var = &theta;
    break;
  case 3:
    var = &v;
    break;
  case 4:
    var = &desiredV;
    break;
  }

  return *var;
}