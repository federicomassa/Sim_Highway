Action.o: Action.cpp Action.h
ActionManager.o: ActionManager.cpp ActionManager.h Action.h LeftAction.h List.h EndLine.h Iterator.h \
	State.h systemParms.h systemTypes.h utility.h Vector.h
EndLine.o: EndLine.cpp EndLine.h
LeftAction.o: LeftAction.cpp LeftAction.h Action.h
State.o: State.cpp State.h systemParms.h systemTypes.h utility.h Vector.h EndLine.h
systemTypes.o: systemTypes.cpp systemTypes.h systemParms.h \
  utility.h Vector.h EndLine.h Failure.h List.h Iterator.h
utility.o: utility.cpp utility.h
testAction.o: testAction.cpp Action.h LeftAction.h ActionManager.h
