
#include "Configuration.h"
#include <iostream>
#include <string>

using namespace std;

Configuration::Configuration(const string& fileName)
{
    debug = false;
    nSteps = 0;
    allMonitorsActive = false;
    allReputationManagersActive = false;
    allTargetsActive = false;
    monitorsNeedLock = false;
    splitView = false;
    cRadius = 100;
    cProb = 1;
    nTimeSteps = 1;
    communicate = false;
    commInterval = 1;
    makeVideo = false;
    saveSubjectiveVisions = false;
    saveVideoImages = false;
    saveConsensusImages = false;
    savePredictionImages = false;
    saveTxtOutput = false;
    subjID = 0;
    parseConf(fileName);
}

void Configuration::parseConf(const string& fileName)
{
    int line = 0;
	/* read Simulator's configuration file */
	ifstream inFile;
	inFile.open(fileName.c_str(), ios::in);
	/* error handling */
	if(!inFile.is_open())
		error("Main", "cannot open " + fileName);
    while(!inFile.eof())
    {
        string str;
		getline(inFile, str, '\n');
        line++;
        stringstream token(str);
		string tmpS;
		token >> tmpS;
        
        if(tmpS[0] == '#' || tmpS == "")
            continue;
        if(tmpS == "debug")
        {
            token >> str;
            debug = (str == "ON");
            continue;
        }
	if(tmpS == "simulation_steps")
	  {
	    token >> nSteps;
            continue;
	  }
        if(tmpS == "communicate")
	  {
            token >> str;
            communicate = (str == "ON");
            continue;
	  }
        if(tmpS == "communication_interval")
	  {
            token >> commInterval;
            continue;
	  }
        if(tmpS == "make_video")
        {
            token >> str;
            makeVideo = (str == "ON");
            continue;
        }
        if(tmpS == "save_subjective_visions")
        {
            token >> str;
            saveSubjectiveVisions = (str == "ON");
            continue;
        }
        if(tmpS == "save_video_images")
        {
            token >> str;
            saveVideoImages = (str == "ON");
            continue;
        }
        if(tmpS == "save_consensus_images")
        {
            token >> str;
            saveConsensusImages = (str == "ON");
            continue;
        }
	if(tmpS == "save_prediction_images")
	{
            token >> str;
            savePredictionImages = (str == "ON");
            continue;
	}

        if(tmpS == "save_txt_output")
        {
            token >> str;
            saveTxtOutput = (str == "ON");
            continue;
        }
        if(tmpS == "show_images")
        {
            token >> str;
            showImages = (str == "ON");
            continue;
        }
        if(tmpS == "channel_radius")
        {
            token >> cRadius;
            continue;
        }
        if(tmpS == "channel_probability")
        {
            token >> cProb;
            continue;
        }
        if(tmpS == "prediction_time_steps")
        {
            token >> nTimeSteps;
            continue;
        }
        if(tmpS == "subject_vehicle")
        {
            token >> subjID;
            if(subjID < 0)
                error("Configuration", "subjID cannot be negative");
            continue;
        }
        if(tmpS == "split_view")
        {
            token >> str;
            splitView = (str == "ON");
            continue;
        }
        if(tmpS == "monitors_need_lock")
        {
            token >> str;
            monitorsNeedLock = (str == "ON");
            continue;
        }
	if(tmpS == "states")
		{
            while(!inFile.eof())
            {
                getline(inFile, str, '\n');
                line++;
                if(str.empty())
                    break;
                if(str[0] == '#')
                    continue;
                qList.insTail(State(str));
            }
            continue;
		}
	if(tmpS == "monitors")
		{
            token >> tmpS;
            if(tmpS == "ALL")
            {
                allMonitorsActive = true;
                activeMonitors.purge();
                continue;
            }
            while(!inFile.eof())
            {
                getline(inFile, str, '\n');
                line++;
                if(str.empty())
                    break;
                if(str[0] == '#')
                    continue;
                stringstream token(str);
                int index;
                token >> index;
                activeMonitors.insTail(index);
            }
            continue;
		}
	if(tmpS == "reputation_managers")
		{
            token >> tmpS;
            if(tmpS == "ALL")
            {
                allReputationManagersActive = true;
                activeReputationManagers.purge();
                continue;
            }
            while(!inFile.eof())
            {
                getline(inFile, str, '\n');
                line++;
                if(str.empty())
                    break;
                if(str[0] == '#')
                    continue;
                stringstream token(str);
                int index;
                token >> index;
                activeReputationManagers.insTail(index);
            }
            continue;
		}
	if(tmpS == "targets")
		{
            token >> tmpS;
            if(tmpS == "ALL")
            {
                allTargetsActive = true;
                activeTargets.purge();
                continue;
            }
            while(!inFile.eof())
            {
                getline(inFile, str, '\n');
                line++;
                if(str.empty())
                    break;
                if(str[0] == '#')
                    continue;
                stringstream token(str);
                int index;
                token >> index;
                activeTargets.insTail(index);
            }
            continue;
		}
     	if(tmpS == "failures")
	  {
            while(!inFile.eof())
	      {
                getline(inFile, str, '\n');
                line++;
                if(str.empty())
                    break;
                if(str[0] == '#')
                    continue;
                Failure f(str);
                /* error handling */
                if(!failures.insTail(f, true))
                {
                    string s = "failure for agent " + toString(f.idx)
                             + " defined twice (line " + toString(line) + ')';
                    error("Configuration", s);
                }
            }
            continue;
		}
        /* default */
        error("Configuration", "parse error at line " + toString(line));
    }
    const int nV = qList.count();
    if(allMonitorsActive)
        for(int i = 0; i < nV; i++)
            activeMonitors.insTail(i);
    if(allReputationManagersActive)
        for(int i = 0; i < nV; i++)
            activeReputationManagers.insTail(i);
    if(allTargetsActive)
        for(int i = 0; i < nV; i++)
            activeTargets.insTail(i);
}

ostream& operator<<(ostream& os, const Configuration& c)
{
    os << "Configuration {" << EndLine(EndLine::INC);
    os << "Debug: " << (c.debug ? "ON" : "OFF") << EndLine();
    os << "Simulation steps: " << c.nSteps << EndLine();
    os << "States: " << EndLine(EndLine::INC);
    os << c.qList << EndLine(EndLine::DEC);
    os << "Failures: " << EndLine(EndLine::INC);
    os << c.failures << EndLine(EndLine::DEC);
    if(c.allMonitorsActive)
        os << "Active monitors: ALL" << EndLine();
    else
    {
        os << "Active monitors:" << EndLine(EndLine::INC);
        os << c.activeMonitors << EndLine(EndLine::DEC);
    }
    os << "Monitors need lock: " << (c.monitorsNeedLock ? "ON" : "OFF");
    os << EndLine();
    if(c.allReputationManagersActive)
        os << "Active reputation managers: ALL" << EndLine();
    else
    {
        os << "Active reputation managers:" << EndLine(EndLine::INC);
        os << c.activeReputationManagers << EndLine(EndLine::DEC);
    }
    if(c.allTargetsActive)
        os << "Active targets: ALL" << EndLine();
    else
    {
        os << "Active targets:" << EndLine(EndLine::INC);
        os << c.activeTargets << EndLine(EndLine::DEC);
    }
    os << "Communication: " << (c.communicate ? "ON" : "OFF") << EndLine();
    if(c.communicate)
    {
        os << "Communication interval: " << c.commInterval << EndLine();
        os << "Channel radius: " << c.cRadius << EndLine();
        os << "Channel probability: " << c.cProb << EndLine();
        os << "Save consensus images: ";
        os << (c.saveConsensusImages ? "ON" : "OFF") << EndLine();
    }
    os << "Prediction simulation steps: " << c.nTimeSteps << EndLine();
    os << "Video output: " << (c.makeVideo ? "ON" : "OFF") << EndLine();
    os << "Split view: " << (c.splitView ? "ON" : "OFF") << EndLine();
    if(c.splitView)
        os << "Subject vehicle ID: " << c.subjID << EndLine();
    os << "Save subjective visions: "
       << (c.saveSubjectiveVisions ? "ON" : "OFF") << EndLine();
    os << "Save video images: " << (c.saveVideoImages ? "ON" : "OFF")
       << EndLine();
    os << "Save txt output: " << (c.saveTxtOutput ? "ON" : "OFF")
        << EndLine();
    os << "Show images: " << (c.showImages ? "ON" : "OFF")
       << EndLine(EndLine::DEC);
    os << '}';
    
    return os;
}
