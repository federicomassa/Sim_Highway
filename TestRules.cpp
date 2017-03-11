#include "TestRules.h"
#include "ruleFunctions.h"

using namespace ruleFunctions;

void TestRules::build()
{
  addLeftRules();
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

  addRule("LeftLaneChange", eList, "Cannot go left is nobody is in front of you");

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
