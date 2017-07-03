CC=g++

CXXFLAGS=-g -O3 -Wall -Wuninitialized -Wunused -pedantic -fpermissive -std=c++11

LDFLAGS= -L/usr/lib/x86_64-linux-gnu -lgd -lopencv_core -lopencv_imgproc -lopencv_calib3d -lopencv_video -lopencv_features2d -lopencv_ml -lopencv_highgui -lopencv_objdetect -lopencv_contrib -lopencv_legacy 

CPPFLAGS='-I/usr/include' 

#####LDFLAGS=-lgd -lcxcore -lhighgui

OBJECTS:=$(patsubst %.cpp,%.o,$(wildcard *.cpp))

Simulator: $(OBJECTS)
	$(CC) $(CXXFLAGS) -o Simulator $(OBJECTS) $(LDFLAGS)

#dep.mk: *.cpp *.h
#	$(CC) -MM *.cpp > dep.mk

include dep.mk

.PHONY: clean

clean:
	rm -rf *.o Simulator Output *~

clean2:
	rm -rf Output 

depMaker:
	g++ -std=c++11 -o dependencyMaker DepMaker/dependencyMaker.cpp -lboost_filesystem -lboost_system

.PHONY: tar

tar:
	tar -czf 'Simulator'.`date +%Y%m%d`.src.tgz *.cpp *.h Makefile Input dep.mk *.m
