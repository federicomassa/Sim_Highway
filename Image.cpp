#include "Image.h"
#include "systemTypes.h"

void Image::cp(const Image& im)
{
    if(im.frame == NULL)
        frame = NULL;
    else
    {
        frame = gdImageCreateTrueColor(im.frame->sx, im.frame->sy);
        gdImageCopy(frame, im.frame, 0, 0, 0, 0,
                    im.frame->sx, im.frame->sy);
    }
    cameraX = im.cameraX;
    cTime = im.cTime;
    fileName = im.fileName;
}        

Image& Image::operator=(const Image& im)
{
    if(this != &im)
    {
        close();
        cp(im);
    }
    
    return *this;
}

void Image::init(double cC)
{
    /* calculating camera left bound from camera center 'cC' */
    cameraX = cC - (double)FRAME_W / (double)SCALE / 2.0;
    cTime = now;
    open();
}

void Image::open()
{
    close();
    
    int offset, white, grey;
    
    frame = gdImageCreateTrueColor(FRAME_W, FRAME_H);
    
    /* error handling */
    if (frame == NULL)
        error("Image::open", "cannot allocate mew image");
    
    white = gdImageColorResolve(frame, 255, 255, 255);
    gdImageFill(frame, 0, 0, white);

    grey = gdImageColorResolve(frame, 153, 153, 153);

    gdImageFilledRectangle(frame, 0, 4, FRAME_W, 7, grey);
    gdImageFilledRectangle(frame, 0, FRAME_H - 8, FRAME_W, FRAME_H - 5, grey);

    offset = - 25 - (int)round(cameraX * SCALE) % 25;
    for(int i = 0; (offset + i * 25) < FRAME_W; i++)
        for(int j = 1; j <= MAX_LANE; j++)
            gdImageFilledRectangle(frame, offset + i * 25,
                                   Y_OFFSET - SCALE * j - 1,
                                   offset + i * 25 + 10,
                                   Y_OFFSET - SCALE * j + 1, grey);
}

void Image::close()
{
    if (frame != NULL)
    {
        gdImageDestroy(frame);
        frame = NULL;
    }
}

void Image::writeFrameNumber(int n)
{
    /* error handling */
    if (frame == NULL)
        error("Image::writeFrameNumber", "frame is NULL");
    
    if(n == -1)
        n = cTime;
    string cTimeString = toString(n, 4);
    
    int white, black, brect[8];
    
    black = gdImageColorResolve(frame, 0, 0, 0);
    white = gdImageColorResolve(frame, 255, 255, 255);
    const int x1 = FRAME_W - 36;
    const int x2 = FRAME_W - 2;
    const int y1 = 1;
    const int y2 = 15;
    gdImageFilledRectangle(frame, x1, y1, x2, y2, white);
    gdImageRectangle(frame, x1, y1, x2, y2, black);
    gdImageStringFT(NULL, brect, black, FONT_NAME,
                    10, 0, 0, 0, (char*)cTimeString.c_str());
    gdImageStringFT(frame, brect, black, FONT_NAME, 10, 0,
                    x1 + (int)round((double)(35 - brect[2] + brect[0]) / 2.0),
                    y1 + 12, (char*)cTimeString.c_str());
}

void Image::writeTransition(const Neighborhood& n)
{
    /* error handling */
    if (frame == NULL)
        error("Image::writeTransition", "frame is NULL");
        
    int white, black, brect[8];
    
    black = gdImageColorResolve(frame, 0, 0, 0);
    white = gdImageColorResolve(frame, 255, 255, 255);
    const int x1 = FRAME_W/100*3;
    const int x2 = FRAME_W/100*25;
    const int y1 = 1;
    const int y2 = 15;

    string transition = "";
    transition = transition + n.targetLastManeuver;
    transition = transition + " => ";
    transition = transition + n.targetManeuver;


    
    gdImageFilledRectangle(frame, x1, y1, x2, y2, white);
    gdImageRectangle(frame, x1, y1, x2, y2, black);
    gdImageStringFT(NULL, brect, black, FONT_NAME,
                    10, 0, x1, y1, (char*)transition.c_str());
    gdImageStringFT(frame, brect, black, FONT_NAME, 10, 0,
                    x1 + (int)round((double)(x2-x1 + 1 - brect[2] + brect[0]) / 2.0),
                    y1 + 12, (char*)transition.c_str());
}


string Image::save(const string prefix, const string suffix)
{
    /* error handling */
    if (frame == NULL)
        error("Image::save", "frame is NULL");
    
    /* creating file name */
    fileName = (string)OUTPUT_DIR + "/" + prefix + toString(cTime, 5)
        + suffix + ".png";
    FILE *fp;
    /* save to disk */
    fp = fopen(fileName.c_str(), "wb");
    /* error handling */
    if (fp == NULL)
        error("Image::save", "cannot write on file: " + fileName);
    gdImagePng(frame, fp);
    fclose(fp);
    
    return fileName;
}

void Image::drawVehicle(const State& q, const Maneuver m, int index,
                        bool isSubject, RepLevel rLev)
{
    /* error handling */
    if (frame == NULL)
        error("Image::addVehicle", "frame is NULL");
    
    gdImagePtr aux, vImg;
    string idStr;
    int brect[8];
    FILE *fp = NULL;

    string tmpS;
    switch(m)
    {
        case FAST:
            tmpS = INPUT(vehicle.png);
            break;
        case SLOW:
            tmpS = INPUT(vehicle_slow.png);
            break;
        case LEFT:
            tmpS = INPUT(vehicle_left.png);
            break;
        case RIGHT:
            tmpS = INPUT(vehicle_right.png);
            break;
        case PLATOON:
  	    tmpS = INPUT(vehicle_platoon.png);
	    break;
        default:
            error("Image::addVehicle", "maneuver UNKNOWN passed");
    }
    fp = fopen(tmpS.c_str(), "rb");
    /* error handling */
    if (fp == NULL)
        error("Image::addVehicle", "cannot open png file: " + tmpS);
    vImg = gdImageCreateFromPng(fp);
    fclose(fp);
    
    if(index != -1)
    {
        int text_size;
        if(vImg->sx > vImg->sy)
            text_size = (int)round((double)vImg->sy * VEHICLE_TXT_SIZE);
        else
            text_size = (int)floor((double)vImg->sx * VEHICLE_TXT_SIZE);
        
        const int circle_size = (int)ceil((double)text_size * 1.8);
        const int text_color = gdImageColorResolve(vImg, 0, 0, 0);
        int circle_color = 0;
        if(isSubject)
            circle_color = gdImageColorResolve(vImg, 152, 200, 255);
        else
            switch(rLev)
            {
                case CORRECT:
                    circle_color = gdImageColorResolve(vImg, 0, 255, 0);
                    break;
                case FAULTY:
                    circle_color = gdImageColorResolve(vImg, 255, 0, 0);
                    break;
                case UNCERTAIN:
                    circle_color = gdImageColorResolve(vImg, 255, 204, 0);
                    break;
                case UNSET:
                    circle_color = gdImageColorResolve(vImg, 255, 255, 255);
                    break;
            }
        gdImageFilledEllipse(vImg, (int)round((double)vImg->sx / 2.0),
                             (int)round((double)vImg->sy / 2.0),
                             circle_size, circle_size, circle_color);
        idStr = toString(index, 2);
        gdImageStringFT(NULL, brect, text_color, FONT_NAME,
                        text_size, 0, 0, 0, (char*)idStr.c_str());
        gdImageStringFT(vImg, brect, text_color, FONT_NAME, text_size, 0,
                        (int)round((double)vImg->sx / 2.0 - (double)brect[2] / 2.0),
                        (int)round((double)vImg->sy / 2.0 - (double)brect[7] / 2.0),
                        (char*)idStr.c_str());
    }
    
    const double scale_x = (double)VEHICLE_IMG_W / (double)vImg->sx;
    const double scale_y = (double)VEHICLE_IMG_H / (double)vImg->sy;
    
    fp = fopen(INPUT(transparent.png), "rb");
    /* error handling */
    if (fp == NULL)
        error("Image::addVehicle", "cannot open png file: transparent.png");
    aux = gdImageCreateFromPng(fp);
    fclose(fp);
    const int dest_w = (int)round(aux->sx * scale_x);
    const int dest_h = (int)round(aux->sy * scale_y);
    gdImageCopyRotated(aux, vImg, aux->sx / 2.0, aux->sy / 2.0, 0, 0,
                       vImg->sx, vImg->sy, (int)round(q.theta / PI * 180));
    gdImageCopyResampled(frame, aux,
                         (int)round((q.x - cameraX) * SCALE - (double)dest_w / 2.0),
                         (int)round(Y_OFFSET - q.y * SCALE - (double)dest_h / 2.0),
                         0, 0, dest_w, dest_h, aux->sx, aux->sy);
    gdImageDestroy(aux);
    gdImageDestroy(vImg);
}

void Image::addVehicle(const Vehicle& v, bool isSubject)
{
    const State q = v.getQ();
    const int id = v.getID();
    const Maneuver m = v.getManeuver();
    drawVehicle(q, m, id, isSubject);
}

void Image::addAllVehicles(const Environment& env)
{
    for(int i = 0; i < env.nV; i++)
        addVehicle(env.v[i]);
}

void Image::addVisibleVehicles(int index, const Environment& env)
{
    /* error handling */
    if (frame == NULL)
        error("Image::addVisibleVehicles", "frame is NULL");

    Area obs;
    env.observableArea(index, obs);
    for(int i = 0; i < env.nV; i++)
      if(env.v[i].inArea(obs)) {
	if (i == index)
	  addVehicle(env.v[i], true);
	else
	  addVehicle(env.v[i]);
      }
}

void Image::addMonitoredVehicles(int index, const Environment& env)
{
    /* error handling */
    if (frame == NULL)
        error("Image::addMonitoredVehicles", "frame is NULL");

    List<Reputation> repList;
    env.v[index].getAgentReputationList(repList);
    Iterator<Reputation> ri(repList);
    Reputation r;
    while(ri(r))
    {
        const Monitor* mon = env.v[index].mLayer.lookFor(r.targetID);
        /* error handling */
        if(mon == NULL)
            error("Image::addMonitoredVehicles", "monitor not found");
        const Maneuver m = mon->targetManeuver;
        drawVehicle(r.qTarget, m, r.targetID, false, r.level);
    }
}

void Image::addArea(const Area& a, const int r, const int g, const int b,
                    const bool invert)
{
    /* error handling */
    if (frame == NULL)
        error("Image::addArea", "frame is NULL");

    if (a.isEmpty() && !invert)
        return;

    gdImagePtr image = gdImageCreate(FRAME_W, FRAME_H);
    /* error handling */
    if (image == NULL)
        error("Image::addArea", "cannot allocate mew image");
    const int white = gdImageColorAllocate(image, 255, 255, 255);
    const int color = gdImageColorAllocate(image, r, g, b);
    //const int black = gdImageColorAllocate(image, 0, 0, 0);
    gdImageFill(image, 0, 0, white);
    
    Matrix_2x2 bounds;
    bounds[0][0] = cameraX;
    bounds[0][1] = cameraX + (double)FRAME_W / (double)SCALE;
    bounds[1][0] = (double)MIN_LANE;
    bounds[1][1] = (double)MAX_LANE + 1.0;
    Area drawA;
    drawA.addRect(bounds);
    
    if (invert)
        drawA -= a;
    else
        drawA *= a;

    List<Rectangle> tmpL;
    drawA.getRectList(tmpL);
    Rectangle re;
    Iterator<Rectangle> i(tmpL);
    while (i(re))
    {
        re.getBounds(bounds);
        const int width = (int)round(SCALE * (bounds[0][1] - bounds[0][0]));
        const int height = (int)round(SCALE * (bounds[1][1] - bounds[1][0]));
        const int x = (int)round((bounds[0][0] - cameraX) * SCALE);
        const int y = (int)round(Y_OFFSET - bounds[1][1] * SCALE);
        gdImageFilledRectangle(image, x, y, x + width, y + height, color);
        //gdImageRectangle(image, x, y, x + width, y + height, black);
    }
    
    gdImageColorTransparent(image, white);
    gdImageCopyMerge(frame, image, 0, 0, 0, 0, FRAME_W, FRAME_H, AREA_OPACITY);
}

void Image::addHiddenArea(int index, const Environment& env)
{
    Area a;
    env.observableArea(index, a);
    /* adds a grey area */
    addArea(a, 35, 35, 35, true);
}

void Image::joinWith(const Image& i2)
{
    /* error handling */
    if (frame == NULL || i2.frame == NULL)
        error("Image::joinWith", "try to join NULL images");
    
    gdImagePtr newFrame =
        gdImageCreateTrueColor(frame->sx, frame->sy + frame->sy);
    /* error handling */
    if (frame == NULL)
        error("Image::joinWith", "cannot allocate mew image");
    
    gdImageCopy(newFrame, frame, 0, 0, 0, 0, frame->sx, frame->sy) ;
    gdImageCopy(newFrame, i2.frame, 0, frame->sy,
                0, 0, i2.frame->sx, i2.frame->sy);
    gdImageDestroy(frame);
    frame = newFrame;
}

void Image::addHypothesis(const Hypothesis& hyp)
{
    addArea(hyp.negative, 0, 255, 0);
    Iterator<Hypothesis::SubHypothesis> shi(hyp.subHypList);
    Hypothesis::SubHypothesis sHyp;
    while(shi(sHyp))
    {
        addArea(sHyp.positive, 255, 0, 0);
    }
}

void Image::addHypothesis(int index, const Environment& env)
{
    List<Hypothesis> hList;
    env.getHypothesis(index, hList);
    Iterator<Hypothesis> i(hList);
    Hypothesis hyp;
    while(i(hyp))
        addHypothesis(hyp);
}

void Image::drawNeighborhood(const Neighborhood& n)
{  
  Vector<double, 2> q = n.qTarget.toPoint();
  /* center image on target x coordinate */
  init(q[0]);
  open();
  const RepLevel rLev = n.getTargetReputation();
  
  drawVehicle(n.qTarget, FAST, n.targetID, false, rLev);
  /* draw other vehicles */
  Iterator<State> qi(n.qList);
  State tmpQ;
  while(qi(tmpQ))
    drawVehicle(tmpQ);
  /* draw frame number */
  writeFrameNumber(now - 1);

  /* draw transition*/
  writeTransition(n);
  
  /* draw hypothesis */
  if(rLev != FAULTY)
    {
      Image copyImg = *this;
      Iterator<Hypothesis> hi(n.lastHypLists[n.targetManeuver]);
      Hypothesis tmpH;
      hi(tmpH);

      addHypothesis(tmpH);
      while(hi(tmpH))
        {
	  Image tmpImage = copyImg;
	  tmpImage.addHypothesis(tmpH);
	  tmpImage.writeFrameNumber(now - 1);
	  joinWith(tmpImage);
        }
    }
}

void Image::drawNeighborhood(const Neighborhood& n, const State& q, Maneuver m,
                             int index)
{
    drawNeighborhood(n);
    drawVehicle(q, m, index, true);
}

void Image::saveConsensusImages(const Environment& env,
                                const State lastStates[], int cStep)
{
    for(int i = 0; i < env.nV; i++)
    {
        List<Neighborhood> nList;
        env.v[i].getNeighborhoodList(nList);
        Iterator<Neighborhood> ni(nList);
        Neighborhood tmpN;
        while(ni(tmpN))
        {
            drawNeighborhood(tmpN, lastStates[i], FAST, i);
            string suffix = "-A" + toString(i, 2) + "-T"
                          + toString(tmpN.targetID, 2) + "-CS"
                          + toString(cStep, 2);
            save('C', suffix);
        }
    }
}

