
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <fstream>

#include "EndLine.h"
#include "List.h"
#include "systemTypes.h"
#include "utility.h"
#include "State.h"

class Configuration
{
    friend ostream& operator<<(ostream& os, const Configuration& c);
    void parseConf(const string& fileName);
public:
    /* debug */
    bool debug;
    /* simulation steps */
    int nSteps;
    /* vehicle list */
    List<State> qList;
    /* subject vehicle */
    int subjID;
    /* active monitors list */
    List<int> activeMonitors;
    bool allMonitorsActive;
    bool monitorsNeedLock;
    /* active reputation managers list */
    List<int> activeReputationManagers;
    bool allReputationManagersActive;
    /* active target list */
    List<int> activeTargets;
    bool allTargetsActive;
    /* split view */
    bool splitView;
    /* communication is active */
    bool communicate;
    /* agents communicate every commInterval steps */
    int commInterval;
    /* channel parameters */
    double cRadius;
    double cProb;
    /* other parameters */
    bool makeVideo;
    bool saveVideoImages;
    bool saveConsensusImages;
    bool saveSubjectiveVisions;
    bool saveTxtOutput;
    bool showImages;
    /* failures */
    List<Failure> failures;
    /* constructor */
    Configuration(const string& fileName);
    /* destructor */
    ~Configuration() { }
};

#endif

