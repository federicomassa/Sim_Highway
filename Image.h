
#ifndef IMAGE_H
#define IMAGE_H

#include <gd.h>

#include "definition.h"
#include "Environment.h"

extern int now;

class Image
{
    gdImagePtr frame;
    double cameraX;
    /* creation time */
    int cTime;
    /* filename */
    string fileName;

    void init(double);
    void open();
    void close();
    /* draw a vehicle */
    void addVehicle(const Vehicle&, bool isSubject = false);
    void drawVehicle(const State& q, const Maneuver m = FAST, int index = -1,
                     bool isSubject = false, RepLevel rLev = UNSET);
    void addHypothesis(const Hypothesis& hyp);
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
    string save(const char prefix = 'F', const string suffix = "")
    {
        string sprefix(1, prefix);
        return save(sprefix, suffix);
    }
    string save(const string prefix, const string suffix = "");
    /* get creation time */
    int getCTime() const { return cTime; }
    /* write frame number in upper right corner */
    void writeFrameNumber(int n = -1);
    /* write transition at the top */
    void writeTransition(const Neighborhood&);
    /* draw all vehicles */
    void addAllVehicles(const Environment&);
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
    void saveConsensusImages(const Environment& env, const State lastStates[],
                             int cStep = 0);
};

#endif
