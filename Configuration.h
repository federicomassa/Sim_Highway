
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <fstream>
#include <utility>

#include "EndLine.h"
#include "List.h"
#include "systemTypes.h"
#include "utility.h"
#include "State.h"

class Configuration
{
    friend std::ostream& operator<<(std::ostream& os, const Configuration& c);
    void parseConf(const std::string& fileName);
public:
    /* debug */
    bool debug;
    /* simulation steps */
    int nSteps;
    /* vehicle list */
    List<std::pair<State, Parms> > qpList;
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
    /* active senders list */
    List<int> activeSenders;
    
    bool allTargetsActive;
    bool allSendersActive;
    /* split view */
    bool splitView;
    /* communication is active */
    bool communicate;
    /* agents communicate every commInterval steps */
    int commInterval;
    /* channel parameters */
    double cRadius;
    double cProb;

    /* prediction span */
    int nTimeSteps;
    /* other parameters */
    bool makeVideo;
    bool makeActionVideo;
    bool makeMonitorVideo;
    bool saveVideoImages;
    bool saveConsensusImages;
    int  saveConsensusTimeMin;
    int  saveConsensusTimeMax;
    bool savePredictionImages;
    bool saveSubjectiveVisions;
    bool saveTxtOutput;
    bool showImages;
    /* failures */
    List<Failure> failures;
    /* constructor */
    Configuration(const std::string& fileName);
    /* destructor */
    ~Configuration() { }
};

#endif

