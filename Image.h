
#ifndef IMAGE_H
#define IMAGE_H

#include <gd.h>
#include <string>

#include "definition.h"
#include "systemTypes.h"
#include "Reputation.h"

extern int now;

class Environment;
class Vehicle;
class State;
class Hypothesis;
class Area;
class Neighborhood;

class Image
{
    gdImagePtr frame;
    double cameraX;
    /* creation time */
    int cTime;
    /* filename */
    std::string fileName;

    void init(double);
    void open();
    void close();
    /* draw a vehicle */
    void addVehicle(const Vehicle&, bool isSubject = false);
    void drawVehicle(const Sensing& s, const Maneuver m = FAST, int index = -1,
                     bool isSubject = false, RepLevel rLev = UNSET, const int& pixWidth = -1, const int& pixHeight = -1);
    void drawVehicleWithLabel(const Vehicle&, const char*, const RepLevel& level = UNSET);
    void drawVehicleWithLabel(const Sensing&, const char*, const RepLevel& level = UNSET);
    void addHypothesis(const Hypothesis& hyp, const bool& invert = false);
    void cp(const Image& im);
public:
    /* constructor */
    Image(double cX = 0) { frame = NULL; init(cX); }
    /* destructor */
    ~Image() { close(); }
    /* copy constructor */
    Image(const Image& im) { cp(im); }
    /* operator = */
    Image& operator=(const Image& im);
    /* reset the image */
    void reset(double cX) { init(cX); /* set cameraX */ }
    /* save the image and returns the file name */
    std::string save(const char prefix = 'F', const std::string suffix = "")
    {
        std::string sprefix(1, prefix);
        return save(sprefix, suffix);
    }
    std::string save(const std::string prefix, const std::string suffix = "");
    /* get frame */
    gdImagePtr getFrame() {return frame;}
    /* get creation time */
    int getCTime() const { return cTime; }
    /* write frame number in upper right corner */
    void writeFrameNumber(int n = -1);
    /* write transition at the top */
    void writeTransition(const Maneuver&, const Maneuver&, const bool& invert = false);
    /* draw all vehicles */
    void addAllVehicles(const Environment&);
    /* draw all vehicles highlighting observer and monitor */
    void addAllVehicles(const Environment&, const int&, const int&, const RepLevel& level = UNSET);

    /* draw all visible vehicles from i-th vehicle */
    void addVisibleVehicles(int, const Environment&);
    /* draw all monitored vehicles */
    void addMonitoredVehicles(int index, const Environment& env);
    /* draw an area */
    void addArea(const Area&, const int, const int, const int,
                 const bool invert = false);
    /* draw i-th vehicle's hidden area */
    void addHiddenArea(int, const Environment&);
    /* join vertically this Image whith another one */
    void joinWith(const Image&);
    /* add vehicle hypothesis */
    void addHypothesis(int index, const Environment& env);
    /* draw neighborhood */
    void drawNeighborhood(const Neighborhood& n);
    void drawNeighborhood(const Neighborhood& n,
                          const State& q, Maneuver m, int index);
    void saveConsensusImages(const Environment& env,
                             int cStep = 0, const int& timeMin = -1, const int& timeMax = -1);
    void drawArc(const double&, const double&,
		 const double&, const double&,
		 const int&, const int&);
    void drawTrajectory(const List<State>&);
    void drawArea(const double&, const double&, const double&, const double&, const int&);
    void drawArea(const Area&, const int&);
    void drawString(const char*, const int&, const int& offset = 0);
};

#endif
