
#include "Automaton.h"

using namespace std;
/* Jammer failure makes automaton to switch between FAST and SLOW maneuver
every JAMMER_SWITCHING_INTERVAL simulation steps */
#define JAMMER_SWITCHING_INTERVAL 5

Automaton::Automaton()
{
    initialized = false;
    agentID = -1;
    initRules(subEvents, events, transitions, resetFunctions);
}

void Automaton::init(const State& q)
{
    /* error handling */
  /*    if(initialized)
        error("Automaton", "object initialized twice");
  */
    /* initialization */
    // sigma = SIGMA_0;

    // sigma is defined in the conf file, and is in the state
    if (q.initManeuver.compare("FAST") == 0)
      sigma = FAST;
    else if (q.initManeuver.compare("SLOW") == 0)
      sigma = SLOW;
    else if (q.initManeuver.compare("LEFT") == 0)
      sigma = LEFT;
    else if (q.initManeuver.compare("RIGHT") == 0)
      sigma = RIGHT;
    else if (q.initManeuver.compare("PLATOON") == 0)
      sigma = PLATOON;
    else
      error("Automaton", "Unknown initial maneuver, check configuration file");

    xi = resetFunctions[sigma](q);
    initialized = true;
}

void Automaton::setManeuver(Maneuver m, const State& q)
{
    /* error handling */
    if(!initialized)
        error("Automaton", "object not initialized");

    /* initialization */
    sigma = m;
    xi = resetFunctions[sigma](q);
    if(CONF.debug)
    {
        LOG.s << "Setting Maneuver " << sigma << " with " << q << ':';
        LOG.s << EndLine(EndLine::INC);
        LOG.s << "xi = " << xi << EndLine(EndLine::DEC);
    }
}

void Automaton::run(const State& qSubj, const List<State>& qList)
{
    /* error handling */
    if(!initialized)
        error("Automaton", "object not initialized");

    /* get failure configuration */
    Failure fail;
    CONF.failures.find(Failure(agentID), fail);
    
    if(fail.aFailures.belongs(Failure::JAMMER))
    {
        if(now % JAMMER_SWITCHING_INTERVAL == 0)
        {
            if(sigma == FAST)
                setManeuver(SLOW, qSubj);
            else
                setManeuver(FAST, qSubj);
        }
        return;
    }
    
    if(detectEvents(qSubj, qList, true))
    {
        for(int i = 0; i < N_MANEUVER; i++)
        {
            if(transitions[sigma][i].getValue().omniscientValue == true)
            {
                if ((Maneuver)i != sigma)
                {
                    /* failure handling */
		  if ((Maneuver)sigma == PLATOON &&
		      fail.aFailures.belongs(Failure::STAYPLATOON))
		    break;
		  else if((Maneuver)i == RIGHT
			  && fail.aFailures.belongs(Failure::NORIGHT))
		    break;
		  else if ((Maneuver)i == PLATOON &&
			   fail.aFailures.belongs(Failure::NOPLATOON))
		    break;
		  
                    /* set new maneuver */
                    setManeuver((Maneuver)i, qSubj);
                }
                break;
            }
        }
    }
}

bool Automaton::detectEvents(const State& qSubj, const List<State>& qList,
                             bool omniscient)
{
  
  Maneuver startManeuver = UNKNOWN;
  Maneuver endManeuver = UNKNOWN;
  bool foundTransition = false;
  
    for(int i = 0; i < N_MANEUVER; i++)
    {
        if(CONF.debug)
        {
            LOG.s << "Calling evaluate of Transition ";
            LOG.s << sigma << " => " << (Maneuver)i << EndLine();
        }
        transitions[sigma][i].evaluate(qSubj, xi, qList, omniscient);
        if(omniscient)
        {
            if(transitions[sigma][i].getValue().omniscientValue == true)
            {
                if(CONF.debug)
                {
                    LOG.s << "Detected transition " << sigma << " => ";
                    LOG.s << (Maneuver)i << EndLine();
                }

		// Protect from accidental ambiguity between transitions
		if (!foundTransition)
		  {
		    startManeuver = sigma;
		    endManeuver = (Maneuver)i;
		    foundTransition = true;
		  }
		else
		  {
		    string errorString = "Agent " + toString(agentID) + " found two valid transitions: ";
		    error("Transition", errorString + startManeuver + " ==> " + endManeuver + " and " + sigma + " ==> " + (Maneuver)i);
		  }
            }
        }
        else
        {
            if(transitions[sigma][i].getValue().nonOmniscientValue == T)
            {
                if(CONF.debug)
                {
                    LOG.s << "Detected certain transition " << sigma << " => ";
                    LOG.s << (Maneuver)i << EndLine();
                }

		// Protect from accidental ambiguity between transitions
		if (!foundTransition)
		  {
		    startManeuver = sigma;
		    endManeuver = (Maneuver)i;
		    foundTransition = true;
		  }
		else
		  {
		    string errorString = "Agent " + toString(agentID) + " found two valid transitions: ";
		    // error("Transition", errorString + startManeuver + " ==> " + endManeuver + " and " + sigma + " ==> " + (Maneuver)i);
		  }
		
		
            }
        }
    }
    
    return foundTransition;
}
