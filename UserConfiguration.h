
#ifndef USERCONFIGURATION_H
#define USERCONFIGURATION_H

#include <fstream>
#include <string>
#include <utility>

#include "EndLine.h"
#include "List.h"
#include "systemTypes.h"
#include "utility.h"
#include "State.h"

enum ErrorType {ABSOLUTE, RELATIVE};
struct Uncertainty
{
    Uncertainty()
    {
        value = 0.0;
        type = ABSOLUTE;
    }

    Uncertainty(const double& val, const ErrorType& et)
    {
        if (et == ABSOLUTE)
            value = val;
        else if (et == RELATIVE)
            value = val / 100;

        type = et;
    }

    double value;
    ErrorType type;

    bool operator==(const Uncertainty& u) const
    {
        if (value == u.value && type == u.type)
            return true;
        else
            return false;
    }

    bool operator!=(const Uncertainty& u) const
    {
        return !(*this == u);
    }
};

class UserConfiguration
{
    friend std::ostream& operator<<(std::ostream& os, const UserConfiguration& c);
    void parseConf(const std::string& fileName);

    List<std::string> stateVars;
    List<std::pair<double, double> > stateBoundaries;
    List<int> monitoredAgents;
    List<Uncertainty> uncertainty;
    int predictionTimeSpan;
    List<double> hiddenAgentMapping;
    bool communication;

public:

    /* constructor */
    UserConfiguration(const std::string& fileName);
    /* destructor */
    ~UserConfiguration() { }

    const List<std::string>& getStateVars() const {return stateVars;}
    const List<std::pair<double, double> >& getStateBoundaries() const {return stateBoundaries;}
    const List<int>& getMonitoredAgents() const {return monitoredAgents;}
    const List<Uncertainty>& getUncertainty() const {return uncertainty;}
    const int& getPredictionTimeSpan() const {return predictionTimeSpan;}
    const List<double> getHiddenAgentMapping() const {return hiddenAgentMapping;}
    const bool& isCommunicationActive() const {return communication;}
};

std::ostream& operator<<(const std::ostream& os, const Uncertainty& u);

#endif

