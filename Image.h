
#ifndef IMAGE_H
#define IMAGE_H

#include <gd.h>

#include "definition.h"
#include "Environment.h"

extern int now;

class Image
{
    static const int gridWidth;
    static const int tickWidth;
    static const int tickLength;

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
    void drawVehicle(const State& q, const Maneuver m = FAST, int index = -1,
                     bool isSubject = false, RepLevel rLev = UNSET);
    void addHypothesis(const Hypothesis& hyp, const bool& invert = false);
    void cp(const Image& im);
    int scaleXValue(const double& x);
    int scaleYValue(const double& y);

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
    /* get creation time */
    int getCTime() const { return cTime; }
    /* write frame number in upper right corner */
    void writeFrameNumber(int n = -1);
    /* write transition at the top */
    void writeTransition(const Maneuver&, const Maneuver&, const bool& invert = false);
    /* draw all vehicles */
    void addAllVehicles(const Environment&);
    /* draw all visible vehicles from i-th vehicle */
    void addVisibleVehicles(int, const Environment&);
    /* draw all monitored vehicles */
    void addMonitoredVehicles(int index, const Environment& env);
    /* draw an area */
    void addArea(const Area&, const int, const int, const int,
                 const bool invert = false);
    //! draw mapping area with grid and hidden vehicle hypotheses.
    void addAreaWithHiddenMap(const Area& mappingArea, const List<Tensor5<bool> >& compatibleHypotheses);
    /* draw i-th vehicle's hidden area */
    void addHiddenArea(int, const Environment&);
    /* join vertically this Image whith another one */
    void joinWith(const Image&);
    /* add vehicle hypothesis */
    void addHypothesis(int index, const Environment& env);
    /* draw neighborhood */
    void drawNeighborhood(const Neighborhood& n, const std::pair<Maneuver, Maneuver>& trans = std::make_pair(UNKNOWN, UNKNOWN), const Environment* env = nullptr);
    void drawNeighborhood(const Neighborhood& n,
                          const State& q, Maneuver m, int index, const std::pair<Maneuver, Maneuver>& trans = std::make_pair(UNKNOWN, UNKNOWN), const Environment* env = nullptr);
    void saveConsensusImages(const Environment& env, const State lastStates[],
                             int cStep = 0);
};

#endif
