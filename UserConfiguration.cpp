
#include "UserConfiguration.h"
#include "utility.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <regex>
#include <cstdlib>

using namespace std;

UserConfiguration::UserConfiguration(const string& fileName)
{
    predictionTimeSpan = 0;
    communication = false;

    parseConf(fileName);

    // Here add checks on consistency among config entries
    require(stateVars.count() == stateBoundaries.count(), "UserConfiguration::UserConfiguration", "state_variables and state_boundaries should have the same number of entries");
    require(uncertainty.count() == stateVars.count(), "UserConfiguration::UserConfiguration", "uncertainty and state_variables");
    require(hiddenAgentMapping.count() == stateVars.count(), "UserConfiguration::UserConfiguration", "hidden_agent_mapping and state_variables should have the same number of entries");

}

void UserConfiguration::parseConf(const string& fileName)
{
    int line = 0;
    // read user's configuration file

    // regular expressions used to validate user input
    string singleNumber("(\\s*)([0-9]+)(\\s*)");
    string singleAlphaNum("(\\s*)([a-zA-Z]+[a-zA-Z0-9]*)(\\s*)");
    string realNumber("((\\+|-)?([0-9]+)((\\.?)([0-9]+))?)");
    string posRealNumber("((\\+)?([0-9]+)((\\.?)([0-9]+))?)");
    string singleRealNumber("(\\s*)" + realNumber + "(\\s*)");
    string singlePosRealNumber("(\\s*)" + posRealNumber + "(\\s*)");    
    string doubleRealNumber("(\\s*)" + realNumber + "(\\s+)" + realNumber + "(\\s*)");
    string percentage(singlePosRealNumber + "%");
    string onoff("(\\s*)(ON|OFF)(\\s*)");


    ifstream inFile;
    inFile.open(fileName.c_str(), ios::in);
    /* error handling */
    if (!inFile.is_open())
        error("UserConfiguration::parseConf", "cannot open " + fileName);
    while (!inFile.eof())
    {
        string str;
        getline(inFile, str, '\n');
        line++;
        stringstream token(str);
        string tmpS;
        token >> tmpS;
        token.clear();

        if (tmpS[0] == '#' || tmpS == "")
            continue;

        else if (tmpS == "state_variables")
        {
            while (!inFile.eof())
            {
                getline(inFile, str, '\n');
                line++;

                smatch sm;

                if (str.empty())
                    break;
                if (str[0] == '#')
                    continue;

                if (regex_match(str, sm, regex(singleAlphaNum)))
                    stateVars.insTail(sm.str(2));
                else
                    error("UserConfiguration::parseConf", "<state_variables> entry should contain a valid c++ identifier (alphanumeric combination, first character cannot be a digit)");
            }
        }
        else if (tmpS == "state_boundaries")
        {
            while (!inFile.eof())
            {
                getline(inFile, str, '\n');
                line++;

                smatch sm;

                if (str.empty())
                    break;
                if (str[0] == '#')
                    continue;

                if (regex_match(str, sm, regex(doubleRealNumber)))
                {
                    stateBoundaries.insTail(std::make_pair(atof(sm.str(2).c_str()), atof(sm.str(9).c_str())));
                }
                else
                    error("UserConfiguration::parseConf", "<state_boundaries> entry should contain two numbers in each line");
            }
        }
        else if (tmpS == "monitored_agents")
        {
            while (!inFile.eof())
            {
                getline(inFile, str, '\n');
                line++;

                smatch sm;

                if (str.empty())
                    break;
                if (str[0] == '#')
                    continue;

                if (regex_match(str, sm, regex(singleNumber)))
                    monitoredAgents.insTail(atoi(sm.str(2).c_str()));
                else
                    error("UserConfiguration::parseConf", "<monitored_agents> entry should contain a single number per line");
            }
        }

        else if (tmpS == "uncertainty")
        {
            while (!inFile.eof())
            {
                getline(inFile, str, '\n');
                line++;

                smatch sm;

                if (str.empty())
                    break;
                if (str[0] == '#')
                    continue;

                if (regex_match(str, sm, regex(singlePosRealNumber)))
                    uncertainty.insTail(Uncertainty(atof(sm.str(2).c_str()), ABSOLUTE));
                else if (regex_match(str, sm, regex(percentage)))
                    uncertainty.insTail(Uncertainty(atof(sm.str(2).c_str()), RELATIVE));
                else
                    error("UserConfiguration::parseConf", "<uncertainty> entry should contain a real number and possibly a % sign in each line");
            }
        }
        else if (tmpS == "prediction_time_span")
        {
            getline(inFile, str, '\n');
            line++;

            smatch sm;

            if (str.empty())
                break;
            if (str[0] == '#')
                continue;

            if (regex_match(str, sm, regex(singleNumber)))
                predictionTimeSpan = atoi(sm.str(2).c_str());
            else
                error("UserConfiguration::parseConf", "<prediction_time_span> entry should contain an integer number");

        }
        else if (tmpS == "hidden_agent_mapping")
        {
            while (!inFile.eof())
            {
                getline(inFile, str, '\n');
                line++;

                smatch sm;

                if (str.empty())
                    break;
                if (str[0] == '#')
                    continue;

                if (regex_match(str, sm, regex(singlePosRealNumber)))
                    hiddenAgentMapping.insTail(atof(sm.str(2).c_str()));
                else
                    error("UserConfiguration::parseConf", "<hidden_agent_mapping> entry should contain a list of positive real numbers");
            }
        }
        else if (tmpS == "communication")
        {
            getline(inFile, str, '\n');
            line++;

            smatch sm;

            if (str.empty())
                break;
            if (str[0] == '#')
                continue;

            if (regex_match(str, sm, regex(onoff)))
                {
                    if (sm.str(2) == "ON")
                        communication = true;
                    else if (sm.str(2) == "OFF")
                        communication = false;
                }
            else
                error("UserConfiguration::parseConf", "<communication> entry should contain ON or OFF");

        }
        else
        {
            error("UserConfiguration::parseConf", "Unknown command at line " + toString(line));
        }

    }
}

ostream& operator<<(ostream & os, const UserConfiguration & c)
{
    os << "User Configuration {" << EndLine(EndLine::INC);
    os << "State variables: " << c.stateVars << EndLine();
    os << "State boundaries: " << c.stateBoundaries << EndLine();
    os << "Monitored agents: " << c.monitoredAgents << EndLine();
    os << "Uncertainty: " << c.uncertainty << EndLine();
    os << "Prediction time span: " << c.predictionTimeSpan << EndLine();
    os << "Hidden agent mapping: " << c.hiddenAgentMapping << EndLine();
    os << "Communication: " << ((c.communication) ? "ON" : "OFF") << EndLine(EndLine::DEC);
    os << '}';

    return os;
}

std::ostream& operator<<(std::ostream& os, const Uncertainty& u)
{
    os << ((u.type == RELATIVE) ? u.value * 100 : u.value) << ((u.type == RELATIVE) ? " %" : "");
    return os;
}


// ================= UNIT TEST ================= //

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("UserConfiguration testing", "[UserConfiguration]")
{
    UserConfiguration conf("Input/Test.conf");
    cout << conf << EndLine();

    const List<std::string>& stateVars = conf.getStateVars();
    const std::string* str;
    const std::string* str2;

    List<std::string> correctStateVars;
    correctStateVars.insTail("x");
    correctStateVars.insTail("y");
    correctStateVars.insTail("theta");
    correctStateVars.insTail("v");
    correctStateVars.insTail("desiredV");

    REQUIRE(correctStateVars.count() == stateVars.count());

    for (int i = 0; i < correctStateVars.count(); i++)
    {
        stateVars.getElem(str, i);
        correctStateVars.getElem(str2, i);

        REQUIRE(*str == *str2);
    }

    List<std::pair<double, double> > correctStateBoundaries;
    correctStateBoundaries.insTail(std::make_pair(-1000, 1000.42));
    correctStateBoundaries.insTail(std::make_pair(0.89, 2.13));
    correctStateBoundaries.insTail(std::make_pair(-45.1, 45));
    correctStateBoundaries.insTail(std::make_pair(0, 0.92));
    correctStateBoundaries.insTail(std::make_pair(-14.1, 14.1));

    REQUIRE(correctStateBoundaries.count() == conf.getStateBoundaries().count());

    std::pair<double, double>* correctBoundariesPair;
    std::pair<double, double>* boundariesPair;


    for (int i = 0; i < correctStateBoundaries.count(); i++)
    {
        conf.getStateBoundaries().getElem(boundariesPair, i);
        correctStateBoundaries.getElem(correctBoundariesPair, i);

        REQUIRE(boundariesPair->first == correctBoundariesPair->first);
        REQUIRE(boundariesPair->second == correctBoundariesPair->second);
    }

    // Monitored agents
    const List<int>& monitoredAgents = conf.getMonitoredAgents();
    const int* monitoredAgent;
    const int* correctMonitoredAgent;

    List<int> correctMonitoredAgents;
    correctMonitoredAgents.insTail(1);
    correctMonitoredAgents.insTail(2);
    correctMonitoredAgents.insTail(4);

    REQUIRE(correctMonitoredAgents.count() == monitoredAgents.count());

    for (int i = 0; i < correctMonitoredAgents.count(); i++)
    {
        monitoredAgents.getElem(monitoredAgent, i);
        correctMonitoredAgents.getElem(correctMonitoredAgent, i);

        REQUIRE(*monitoredAgent == *correctMonitoredAgent);
    }

    // Uncertainty
    const List<Uncertainty>& uncertainties = conf.getUncertainty();
    const Uncertainty* uncertainty;
    const Uncertainty* correctUncertainty;

    List<Uncertainty> correctUncertainties;
    correctUncertainties.insTail(Uncertainty(0.5, RELATIVE));
    correctUncertainties.insTail(Uncertainty(42.1, ABSOLUTE));
    correctUncertainties.insTail(Uncertainty(10.2, RELATIVE));
    correctUncertainties.insTail(Uncertainty(0.56, ABSOLUTE));
    correctUncertainties.insTail(Uncertainty(1, RELATIVE));

    REQUIRE(correctUncertainties.count() == uncertainties.count());

    for (int i = 0; i < correctUncertainties.count(); i++)
    {
        uncertainties.getElem(uncertainty, i);
        correctUncertainties.getElem(correctUncertainty, i);

        REQUIRE(*uncertainty == *correctUncertainty);
    }

    // Prediction time span
    REQUIRE(conf.getPredictionTimeSpan() == 10);

    // Hidden agent mapping
    const List<double>& hiddenAgentMappings = conf.getHiddenAgentMapping();
    const double* hiddenAgentMapping;
    const double* correctHiddenAgentMapping;

    List<double> correctHiddenAgentMappings;

    correctHiddenAgentMappings.insTail(1);
    correctHiddenAgentMappings.insTail(0.25);
    correctHiddenAgentMappings.insTail(25);
    correctHiddenAgentMappings.insTail(0.2);
    correctHiddenAgentMappings.insTail(0.2);

    REQUIRE(correctHiddenAgentMappings.count() == hiddenAgentMappings.count());

    for (int i = 0; i < correctHiddenAgentMappings.count(); i++)
    {
        correctHiddenAgentMappings.getElem(correctHiddenAgentMapping, i);
        hiddenAgentMappings.getElem(hiddenAgentMapping, i);

        REQUIRE(*hiddenAgentMapping == *correctHiddenAgentMapping);
    }

      // Communication
    REQUIRE(conf.isCommunicationActive() == true);

}

