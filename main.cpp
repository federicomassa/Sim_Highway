#include <opencv2/highgui/highgui_c.h>
#include <unistd.h>
#include <ctime>

#include "ActionVideoCreator.h"
#include "MonitorVideoCreator.h"
#include "Image.h"
#include "ProgressBar.h"
#include "Configuration.h"
#include "Logger.h"
#include "Environment.h"

using namespace std;
/* initialize Simulator's configuration file */
const Configuration CONF(INPUT(Simulator.conf));

/* global Logger object */
Logger LOG;

/* global monitor log */
Logger monitorLog(OUTPUT_DIR + std::string("/Monitor.log"));

/* Monitor results log */
Logger ResultLog(OUTPUT_DIR + std::string("/Results.log"));

/* time step */
int now = 0;

int main (int argc, char* argv[])
{
    /* set cout modifiers */
    cout << fixed << setprecision(3);
    
    cout << "Simulator running..." << endl << endl;

    /* simulation start time */
    time_t start = time(NULL);
        
    /* new failure handler */
    set_new_handler(exaustedMem);

    /* initialize random number generator */
    srand((unsigned)time(NULL));
    
    /* create output dir if not created yet */
    mkdir(OUTPUT_DIR, S_IRWXU | S_IRGRP | S_IROTH);

    /* number of vehicles */
    const int nV = CONF.qpList.count();

    /* instantiate the environment */

    Environment env(nV, CONF.cRadius, CONF.cProb);
    
    /* vehicles initialization */
    env.initVehicles(CONF.qpList);

    /* monitors initialization */
    Iterator<int> i(CONF.activeMonitors);
    int index;
    while(i(index))
        env.activateMonitorLayer(index);

    /* reputation manager initialization */
    i.initialize(CONF.activeReputationManagers);
    while(i(index))
        env.activateReputationManager(index);

    /* vehicle subject */
    const int subj = CONF.subjID;

    /* openCV image format */
    IplImage* cvImg;
    
    /* open a new window */
    if(CONF.showImages)
        cvNamedWindow("Simulator", CV_WINDOW_AUTOSIZE);
    
    /* initialize video output */
    CvVideoWriter* video;
    ActionVideoCreator actionVideo(0, 1);
    MonitorVideoCreator monitorVideo(0, 1);
    
    if(CONF.makeVideo)
    {
        remove(OUTPUT(output.avi));
        CvSize frameSize;
        if(CONF.splitView)
            frameSize = cvSize(FRAME_W, FRAME_H * 2);
        else
            frameSize = cvSize(FRAME_W, FRAME_H);
        video = cvCreateVideoWriter(OUTPUT(output.avi),
                                    CV_FOURCC('P','I','M','1'),
                                    FPS, frameSize, 1);
    }

    if (CONF.makeActionVideo)
      {
	CvSize frameSize = cvSize(FRAME_W, FRAME_H);
	actionVideo.init(OUTPUT(actionVideo.avi),
			 CV_FOURCC('P', 'I', 'M', '1'),
			 FPS, frameSize, 1);
	actionVideo.setEnvironment(&env);
      }

    if (CONF.makeMonitorVideo)
      {
	CvSize frameSize = cvSize(FRAME_W, FRAME_H);
	monitorVideo.init(OUTPUT(monitorVideo.avi),
			 CV_FOURCC('P', 'I', 'M', '1'),
			 FPS, frameSize, 1);
	monitorVideo.setGlobalEnvironment(&env);
      }

    Image curFrame, prevFrame;
    
    /* subjective visions */
    Image* visions = new Image[nV];
    
    /* vehicles last states and maneuvers */
    State* lastStates = new State[nV];

    /* print CONFiguration status */
    cout << CONF << endl << endl;

    /* print progress bar */
    cout << ProgressBar();

    const bool createFrames = CONF.makeVideo || CONF.saveVideoImages ||
                              CONF.showImages;
    
    for(now = 0; now < CONF.nSteps; now++)
    {      
        /* mark log file */
        if(now > 0 && CONF.debug)
	  {
	    LOG.setFile();
	    monitorLog.setFile();
	  }
        /* update progress bar */
        cout << ProgressBar(now, CONF.nSteps);
        /* simulation step */
        env.run();
        /* txt output */
        if(CONF.saveTxtOutput)
            env.outputVehiclesStates();
        const double cameraCenter = env.getQ(subj).x;
        if(createFrames)
        {
            /* build omniscient frame section */
            curFrame.reset(cameraCenter);
            curFrame.addAllVehicles(env);
            curFrame.writeFrameNumber();
        }

	if (CONF.makeActionVideo)
	  actionVideo.run();

	if (CONF.makeMonitorVideo)
	  {
	    monitorVideo.setEnvironment(env.getVehicleFromID(0)->getMonitorLayer()->getEnvironment());
	    monitorVideo.run();
	  }
	
        /* building subjective visions images */
        if(CONF.saveSubjectiveVisions)
            for(int i = 0; i < nV; i++)
            {
                /* update progress bar */
                cout << ProgressBar(now, CONF.nSteps);
                if(now > 0)
                {
                    visions[i].addHypothesis(i, env);
                    visions[i].addMonitoredVehicles(i, env);
                    string prefix = "A" + toString(i, 2) + "-S";
                    visions[i].save(prefix, "");
                }
                visions[i].reset(env.getQ(i).x);
                visions[i].addVisibleVehicles(i, env);
                visions[i].addHiddenArea(i, env);
                visions[i].writeFrameNumber();
            }
        if(createFrames && CONF.splitView)
        {
            prevFrame.addHypothesis(subj, env);
            prevFrame.addMonitoredVehicles(subj, env);
            prevFrame.writeFrameNumber();
        }
        /* txt output, FIXME: only now > 0?*/
        if(CONF.saveTxtOutput && now > 0)
        {
            /* output neighborhood measures */
            env.outputNeighborhoodsMu(0);
            env.outputTargetsReputation(0);
        }
	
        if(CONF.communicate)
        {
            if(now > 0 && now % CONF.commInterval == 0)
            {
                if(CONF.saveConsensusImages)
                {
                    /* build omniscient neighborhoods */
                    List<Neighborhood> nList;
                    env.omniscientNeighborhoodList(nList);
                    Image tmpImg;
                    Iterator<Neighborhood> ni(nList);
                    Neighborhood tmpN;
                    while(ni(tmpN))
                    {
                        tmpImg.drawNeighborhood(tmpN);
                        string suffix = "-CENTR-T"
                                      + toString(tmpN.getTargetID(), 2);
                        tmpImg.save('C', suffix);
                    }
                }
                /* update progress bar */
                cout << ProgressBar(now, CONF.nSteps);
                /* start consensus */
                if(CONF.debug)
                    LOG.s << "Consensus START..." << EndLine(EndLine::INC);
                if(CONF.saveConsensusImages)
                {
		  Image tmpImg;
		  tmpImg.saveConsensusImages(env, lastStates);
		}
		
                const int totCSteps = 1;
                for(int cStep = 0; cStep < totCSteps; cStep++)
                {
                    if(CONF.debug)
                        LOG.s << "Consensus step: " << cStep << EndLine();
                    /* txt output */
                    if(CONF.saveTxtOutput)
                        env.outputNeighborhoodsOverhead(cStep);
                    env.consensusStep();
                    /* txt output */
                    if(CONF.saveTxtOutput)
		      {
                        env.outputNeighborhoodsMu(cStep + 1);
                        env.outputTargetsReputation(cStep + 1);
		      }
                    /* update progress bar */
                    cout << ProgressBar(now, CONF.nSteps);
                    if(CONF.saveConsensusImages)
                    {
                        Image tmpImg;
                        tmpImg.saveConsensusImages(env, lastStates, cStep + 1);
                    }
                }
                if(CONF.debug)
                    LOG.s << "Consensus END." << EndLine(EndLine::DEC);
                /* end consensus */
            }
	    for(int i = 0; i < nV; i++)
                // store vehicles last state and maneuver 
                lastStates[i] = env.getQ(i);
        }
        /* save real Configuration */
        if(CONF.saveVideoImages && CONF.splitView)
            curFrame.save('R');
        /* join real Configuration with estimated one */
        if(createFrames && CONF.splitView)
            curFrame.joinWith(prevFrame);
        string fileName = "";
        if(createFrames)
            fileName = curFrame.save();
        /* save estimated Configuration */
        if(createFrames && CONF.splitView)
        {
            if(CONF.saveVideoImages)
                prevFrame.save('O');
            prevFrame.reset(cameraCenter);
            prevFrame.addHiddenArea(subj, env);
            prevFrame.addVisibleVehicles(subj, env);
        }
        if(CONF.makeVideo || CONF.showImages)
	  cvImg = cvLoadImage(fileName.c_str());
        if(createFrames && !CONF.saveVideoImages)
	  remove(fileName.c_str());
        if(CONF.showImages)
	  /* show current frame in a window */
	  cvShowImage("Simulator", cvImg);
        if(CONF.makeVideo)
	  /* add current frame to the video */
	  cvWriteFrame(video, cvImg);
        if(CONF.makeVideo || CONF.showImages)
	  cvReleaseImage(&cvImg);
        /* wait for the window to automatically resize */
        if(CONF.showImages)
	  cvWaitKey(20); /* sleep 20 msec */
    }

    /* update progress bar */
    cout << ProgressBar(1, 1);

    /* free dynamic memory */
    delete[] visions;
    delete[] lastStates;
    
    /* close output video file */
    if(CONF.makeVideo)
        cvReleaseVideoWriter(&video);
    /* close output window */
    if(CONF.showImages)
      cvDestroyWindow("Simulator");
    
    cout << endl << endl << "Done." << endl;

    /* simulation stop time */
    time_t stop = time(NULL);
    int seconds = (int)round(difftime(stop, start));
    int hours = seconds / 3600;
    seconds = seconds - hours * 3600;
    int mins = seconds / 60;
    seconds = seconds - mins * 60;
    cout << "Simulation time: ";
    cout << hours << " hours, ";
    cout << mins << " minutes and ";
    cout << seconds << " seconds." << endl;
    
    return 0;
}
