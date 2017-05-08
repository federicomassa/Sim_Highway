#include "TestRules.h"
#include "ruleFunctions.h"

/* FIXME: event and subevent numbers necessary? */

using namespace ruleFunctions;

void TestRules::build()
{
  addSafetyRules();
  addLeftRules();
  addRightRules();
  addRightOvertakeRules();
  addCruiseRules();
}

void TestRules::addSafetyRules()
{
  List<SubEvent*> seList;
  List<Event> eList;
  
  /* ================= Nobody ahead rule ==================== */
  
  Event eSafety;
  SubEvent* sSafety = new SubEvent;
  
  sSafety->init(forwardDangerous, forwardDangerousArea, OR, "Someone is in front of me at a dangerous distance", 0);
  seList.insHead(sSafety);

  /* Transfer subevent ownership to SocialRules */
  eSafety.init(seList, 0);
  eList.insHead(eSafety);

  addRule("Safety", eList, "You are not keeping the safety distance from the vehicle in front of you", CONTINUOUS);

  /* clear for re-use */
  seList.purge();
  eList.purge();

}

void TestRules::addLeftRules()
{

  List<SubEvent*> seList;
  List<Event> eList;
    
  /* ================= Nobody ahead rule ==================== */
  
  Event eForwardFree;
  SubEvent* sForwardFree = new SubEvent;

  sForwardFree->init(forwardBlocking, forwardArea, NOR, "Nobody is in front of me", 0);
  seList.insHead(sForwardFree);

  /* Transfer subevent ownership to SocialRules */
  eForwardFree.init(seList, 0);
  eList.insHead(eForwardFree);

  addRule("LeftLaneChange", eList, "Cannot go left if nobody is in front of you");

  /* clear for re-use */
  seList.purge();
  eList.purge();

  /* =============== Someone on the left rule ================ */

  Event eLeftBlock;
  SubEvent* sLeftBlock = new SubEvent;

  sLeftBlock->init(leftBlocking, leftArea, OR, "Someone is on my left", 1);
  seList.insHead(sLeftBlock);

  /* Transfer subevent ownership to SocialRules */
  eLeftBlock.init(seList, 0);
  eList.insHead(eLeftBlock);

  addRule("LeftLaneChange", eList, "Cannot go left if next lane is blocked");

  /* clear for re-use */
  seList.purge();
  eList.purge();
  
  
}

void TestRules::addRightRules()
{  
  List<SubEvent*> seList;
  List<Event> eList;
    
  /* =============== Someone on the right rule ================ */

  Event eRightBlock;
  SubEvent* sRightBlock = new SubEvent;

  sRightBlock->init(rightBlocking, rightArea, OR, "Someone is on my right", 2);
  seList.insHead(sRightBlock);

  /* Transfer subevent ownership to SocialRules */
  eRightBlock.init(seList, 0);
  eList.insHead(eRightBlock);

  addRule("RightLaneChange", eList, "Cannot go right if that lane is blocked");

  /* clear for re-use */
  seList.purge();
  eList.purge();
  
  
}

void TestRules::addRightOvertakeRules()
{
  /* in these rules a right overtake is always wrong, so write a subevent that always return true */
  List<SubEvent*> seList;
  List<Event> eList;
  
  /* =============== Always true rule ================ */

  Event eTrue;
  SubEvent* sTrue = new SubEvent;
  
  sTrue->init(trueFunction, NULL, SINGLE, "True subevent", 3);
  seList.insHead(sTrue);

  /* Transfer subevent ownership to SocialRules */
  eTrue.init(seList, 0);
  eList.insHead(eTrue);

  addRule("RightOvertake", eList, "Cannot overtake a vehicle from the right");

  /* clear for re-use */
  seList.purge();
  eList.purge();

  
}

void TestRules::addCruiseRules()
{

  List<SubEvent*> seList;
  List<Event> eList;
    
  /* =============== Nobody on the right rule ================ */

  Event eRightFree;
  SubEvent* sRightFree = new SubEvent;
  SubEvent* sNotMinLane = new SubEvent;

  sRightFree->init(rightReturn, rightReturnArea, NOR, "Nobody is on my right", 2);
  seList.insHead(sRightFree);

  sNotMinLane->init(minLane, NULL, NSINGLE, "I'm not on the minimum lane", 2);
  seList.insHead(sNotMinLane);
  
  /* Transfer subevent ownership to SocialRules */
  eRightFree.init(seList, 0);
  eList.insHead(eRightFree);

  addRule("Cruise", eList, "You should go to previous lane", CONTINUOUS);

  /* clear for re-use */
  seList.purge();
  eList.purge();  
  
}



TestRules::~TestRules()
{
  Iterator<Rule> rIt(rList);
  Rule r;

  while (rIt(r))
    {
      Iterator<Event> eIt(r.getEventList());
      Event e;

      while (eIt(e))
	{
	  e.deleteSubEvents();
	}
    }
}
