#include "Image.h"
#include "Predictor.h"
#include "systemTypes.h"
#include <map>

using namespace std;

const int Image::gridWidth = 2;
const int Image::tickWidth = 1;
const int Image::tickLength = 8;

void Image::cp(const Image& im)
{
    if (im.frame == NULL)
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
    if (this != &im)
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
    for (int i = 0; (offset + i * 25) < FRAME_W; i++)
        for (int j = 1; j <= MAX_LANE; j++)
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

    if (n == -1)
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

void Image::writeTransition(const Maneuver& targetLastManeuver, const Maneuver& targetManeuver, const bool& invert)
{
    /* error handling */
    if (frame == NULL)
        error("Image::writeTransition", "frame is NULL");

    int white, black, brect[8];

    black = gdImageColorResolve(frame, 0, 0, 0);
    white = gdImageColorResolve(frame, 255, 255, 255);
    const int x1 = FRAME_W / 100 * 3;
    const int x2 = FRAME_W / 100 * 25;
    const int y1 = 1;
    const int y2 = 15;

    string transition = "";
    transition = transition + targetLastManeuver;
    if (!invert)
        transition = transition + " => ";
    else
        transition = transition + " !=> ";

    transition = transition + targetManeuver;



    gdImageFilledRectangle(frame, x1, y1, x2, y2, white);
    gdImageRectangle(frame, x1, y1, x2, y2, black);
    gdImageStringFT(NULL, brect, black, FONT_NAME,
                    10, 0, x1, y1, (char*)transition.c_str());
    gdImageStringFT(frame, brect, black, FONT_NAME, 10, 0,
                    x1 + (int)round((double)(x2 - x1 + 1 - brect[2] + brect[0]) / 2.0),
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
    switch (m)
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

    if (index != -1)
    {
        int text_size;
        if (vImg->sx > vImg->sy)
            text_size = (int)round((double)vImg->sy * VEHICLE_TXT_SIZE);
        else
            text_size = (int)floor((double)vImg->sx * VEHICLE_TXT_SIZE);

        const int circle_size = (int)ceil((double)text_size * 1.8);
        const int text_color = gdImageColorResolve(vImg, 0, 0, 0);
        int circle_color = 0;
        if (isSubject)
            circle_color = gdImageColorResolve(vImg, 152, 200, 255);
        else
            switch (rLev)
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
    for (int i = 0; i < env.nV; i++)
        addVehicle(env.v[i]);
}

void Image::addVisibleVehicles(int index, const Environment& env)
{
    /* error handling */
    if (frame == NULL)
        error("Image::addVisibleVehicles", "frame is NULL");

    Area obs;
    env.observableArea(index, obs);
    for (int i = 0; i < env.nV; i++)
        if (env.v[i].inArea(obs)) {
            if (i == index)
                addVehicle(env.v[i], true);
            else
                addVehicle(env.v[i]);
        }
}

void Image::addMonitoredVehicles(int index, const Environment& env)
{
    /* dummy function for now

    // error handling
    if (frame == NULL)
        error("Image::addMonitoredVehicles", "frame is NULL");

    List<Reputation> repList;
    env.v[index].getAgentReputationList(repList);
    Iterator<Reputation> ri(repList);
    Reputation r;
    while(ri(r))
    {
        const Monitor* mon = env.v[index].mLayer.lookFor(r.targetID);
        // error handling
        if(mon == NULL)
            error("Image::addMonitoredVehicles", "monitor not found");
        const Maneuver m = mon->targetManeuver;
        drawVehicle(r.qTarget, m, r.targetID, false, r.level);
    }
    */
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
        gdImageCreateTrueColor(frame->sx, frame->sy + i2.frame->sy);
    /* error handling */
    if (frame == NULL)
        error("Image::joinWith", "cannot allocate mew image");

    /* gdImageCopy(newFrame, frame, 0, 0, 0, 0, frame->sx, frame->sy) ;
    gdImageCopy(newFrame, i2.frame, 0, frame->sy,
    0, 0, i2.frame->sx, i2.frame->sy);*/

    gdImageCopy(newFrame, frame, 0, 0, 0, 0, frame->sx, frame->sy) ;
    gdImageCopy(newFrame, i2.frame, 0, frame->sy,
                0, 0, i2.frame->sx, i2.frame->sy);


    gdImageDestroy(frame);
    frame = newFrame;
}

void Image::addHypothesis(const Hypothesis& hyp, const bool& invert)
{
    if (!invert)
        addArea(hyp.negative, 0, 255, 0);
    else
        addArea(hyp.negative, 255, 0, 0);

    Iterator<Hypothesis::SubHypothesis> shi(hyp.subHypList);
    Hypothesis::SubHypothesis sHyp;
    while (shi(sHyp))
    {


        if (!invert)
            addArea(sHyp.positive, 255, 0, 0);
        else
            addArea(sHyp.positive, 0, 255, 0);
    }
}

void Image::addHypothesis(int index, const Environment& env)
{
    List<Hypothesis> hList;
    env.getHypothesis(index, hList);
    Iterator<Hypothesis> i(hList);
    Hypothesis hyp;
    while (i(hyp))
        addHypothesis(hyp);
}

void Image::drawNeighborhood(const Neighborhood& n, const pair<Maneuver, Maneuver>& trans, const Environment* env)
{
    Vector<double, 2> q = n.qTarget.toPoint();
    /* center image on target x coordinate */
    init(q[0]);
    open();

    RepLevel rLev = UNSET;

    //std::cout << "In draw Neighborhood..." << trans.first << " => " << trans.second << std::endl;
    /*
    for (auto hypItr = n.lastHypLists.begin(); hypItr != n.lastHypLists.end(); hypItr++)
    {
        std::cout << '\t' << (*hypItr).first << std::endl;
    }
    */
    //std::cout << "End block" << std::endl;

    const Vector<List<Hypothesis>, N_MANEUVER>* lastHyp = NULL;
    try
    {
        lastHyp = &(n.lastHypLists.at(trans.first));
    }
    catch (std::out_of_range& e)
    {
        error("Image::drawNeighborhood", "Transition is out of range in lastHyp");
    }

    try
    {
        rLev = n.getTargetReputation().at(trans);
    }
    catch (std::out_of_range& e)
    {
        error("Image::drawNeighborhood", "Called drawNeighborhood with transition out of map range");
    }

    drawVehicle(n.qTarget, FAST, n.targetID, false, rLev);
    /* draw other vehicles */
    Iterator<Sensing> si(n.sList);
    Sensing tmpS;

    //probably doesn't write the IDs?
    while (si(tmpS))
        drawVehicle(tmpS.q);
    /* draw frame number */
    writeFrameNumber(now - 1);

    Image copyImg = *this;

    /* draw transition*/
    writeTransition(trans.first, trans.second);

    /* draw hypothesis */
    if (rLev == UNCERTAIN)
    {
        if (trans.first != trans.second)
        {
            Iterator<Hypothesis> hi((*lastHyp)[(int)trans.second]);
            Hypothesis tmpH;
            hi(tmpH);

            addHypothesis(tmpH);

            bool debug = false;
            if (n.agentID == 0 && n.targetID == 1 && trans.first == FAST && trans.second == SLOW)
                debug = true;

            if (debug)
            {
                std::cout << "FOUND! " << tmpH.eventID << " Negative: " << tmpH.negative << std::endl;
                Iterator<Hypothesis::SubHypothesis> subItr(tmpH.subHypList);

                Hypothesis::SubHypothesis sub;
                while (subItr(sub))
                {
                    std::cout << "FOUND! Positive: " << sub.subEventID << " " << sub.positive << std::endl;
                }
            }

            while (hi(tmpH))
            {
                Image tmpImage = copyImg;
                tmpImage.addHypothesis(tmpH);
                tmpImage.writeFrameNumber(now - 1);
                tmpImage.writeTransition(trans.first, trans.second);
                if (env)
                {
                    const Vehicle* veh = env->getVehicleWithID(n.getAgentID());
                    if (veh)
                        tmpImage.addAreaWithHiddenMap(n.getMappingArea(), veh->getMonitorLayer()->lookFor(n.getTargetID())->getHypothesesLeft()[trans.second]);
                }

                joinWith(tmpImage);
            }
        }
        else //same maneuver
        {
            Iterator<Hypothesis> hi((*lastHyp)[trans.first]);
            Hypothesis tmpH;
            hi(tmpH);

            // if uncertain hypothesis, draw all the hypotheses on the remaining transitions, with inverted positive-negative colors.
            if (tmpH.isSameManeuverUncertain)
            {
                for (int i = 0; i < N_MANEUVER; i++)
                {
                    if ((Maneuver)i == trans.first)
                        continue;

                    Iterator<Hypothesis> otherHI((*lastHyp)[i]);
                    Hypothesis otherH;

                    while (otherHI(otherH))
                    {
                        Image tmpImg = copyImg;
                        tmpImg.addHypothesis(otherH);
                        tmpImg.writeFrameNumber(now - 1);
                        tmpImg.writeTransition(trans.first, (Maneuver)i);

                        if (env)
                        {
                            const Vehicle* veh = env->getVehicleWithID(n.getAgentID());
                            if (veh)
                                tmpImg.addAreaWithHiddenMap(n.getMappingArea(), veh->getMonitorLayer()->lookFor(n.getTargetID())->getHypothesesLeft()[trans.second]);
                        }

                        joinWith(tmpImg);
                    }



                }
            }

        }
    }
}

void Image::drawNeighborhood(const Neighborhood& n, const State& q, Maneuver m,
                             int index, const pair<Maneuver, Maneuver>& trans, const Environment* env)
{
    drawNeighborhood(n, trans, env);
    drawVehicle(q, m, index, true);
}

void Image::saveConsensusImages(const Environment& env,
                                /*FIXME dummy */const State lastStates[], int cStep)
{
    /* How to read consensus images:

       Blue vehicles are the observers,
       Red, yellow and green vehicles are the monitors, respectively recognized with
          FAULTY, UNCERTAIN and CORRECT behaviour.

       Each consensum image might contain more than one hypothesis on the transition SIGMA1 => SIGMA2

       If SIGMA1 != SIGMA2
            each hypothesis refers to a different possibility. Red areas represent hypotheses
      on absence of a vehicle with a certain logical function. Green areas, analogously,
      refers to the presence of a vehicle. The transition is true if both green and red
      areas are occupied by vehicles accordingly.


       If SIGMA1 == SIGMA2
            As the transition is true only if all the others are false, the different hypotheses
      represent the non-realization of the transition SIGMA1 => OTHER_SIGMA. The colors of
      the areas have the same meaning as the SIGMA1 != SIGMA2 case (the colors
      of the anti-hypothesis are inverted with respect to the hypothesis), but now, for the
      condition to be true, is sufficient that only one between the red and green areas are
      occupied accordingly to their meaning. This is because !(A && B) = !A || !B

    */

    for (int i = 0; i < env.nV; i++)
    {
        List<Neighborhood> nList;

        /* take nList from reputation manager */
        env.v[i].getNeighborhoodList(nList);

        /* take monitor list from monitorLayer */
        List<Monitor*> mList = env.v[i].getMonitorLayer()->getMonitorList();

        Iterator<Neighborhood> ni(nList);
        Neighborhood tmpN;
        while (ni(tmpN))
        {
            if (tmpN.targetLastManeuver == UNKNOWN)
                continue;

            Iterator<Monitor*> mI(mList);
            Monitor* m;
            State tmpQ(-100, 0, 0, 0);

            while (mI(m))
            {
                if (m->getTargetID() == tmpN.targetID)
                {
                    tmpQ = m->lastPredictionAgentQ;
                    break;
                }
            };

            Image copyImg;
            copyImg.cp(*this);

            map<pair<Maneuver, Maneuver>, RepLevel> rLevel = tmpN.getTargetReputation();

            for (auto hypItr = rLevel.begin(); hypItr != rLevel.end(); hypItr++)
            {
                //std::cout << "Transition " << (*hypItr).first.first << " => " << (*hypItr).first.second << std::endl;
                drawNeighborhood(tmpN, tmpQ, FAST, env.v[i].idx, (*hypItr).first, &env);
                addAreaWithHiddenMap(tmpN.getMappingArea(), env.v[i].getMonitorLayer()->lookFor(tmpN.getTargetID())->getHypothesesLeft()[(*hypItr).first.second]);
                string suffix = "-A" + toString(env.v[i].idx, 2) + "-T"
                                + toString(tmpN.targetID, 2) + "-TR" + maneuverToStr((*hypItr).first.first) +
                                maneuverToStr((*hypItr).first.second) + "-CS"
                                + toString(cStep, 2);

                save('C', suffix);

                //reset for next iteration
                cp(copyImg);
            }
        }
    }
}

void Image::addAreaWithHiddenMap(const Area& mappingArea, const List<Tensor5<bool> >& compatibleHypotheses)
{
    /* error handling */
    if (frame == NULL)
        error("Image::addAreaWithHiddenMap", "frame is NULL");

    if (mappingArea.isEmpty())
        return;

    gdImagePtr image = gdImageCreate(FRAME_W, FRAME_H);

    // another image with another opacity
    gdImagePtr hypImg = gdImageCreate(FRAME_W, FRAME_H);

    /* error handling */
    if (image == NULL)
        error("Image::addAreaWithHiddenMap", "cannot allocate new image");
    const int white = gdImageColorAllocate(image, 255, 255, 255);
    const int white2 = gdImageColorAllocate(hypImg, 255, 255, 255);

    const int black = gdImageColorResolve(image, 0, 0, 0);
    const int blue = gdImageColorResolve(hypImg, 0, 0, 255);
    const int color = gdImageColorAllocate(image, 120, 120, 120);
    //const int black = gdImageColorAllocate(image, 0, 0, 0);
    gdImageFill(image, 0, 0, white);
    gdImageFill(hypImg, 0, 0, white2);

    Matrix_2x2 bounds;
    /*
    bounds[0][0] = cameraX;
    bounds[0][1] = cameraX + (double)FRAME_W / (double)SCALE;
    bounds[1][0] = (double)MIN_LANE;
    bounds[1][1] = (double)MAX_LANE + 1.0;
    Area drawA;
    drawA.addRect(bounds);
    */

    List<Rectangle> tmpL;
    mappingArea.getRectList(tmpL);

    // -1 to account for no hidden vehicles hypothesis
    require(tmpL.count() == (compatibleHypotheses.count() - 1), "Image::addAreaWithHiddenMap", "Incompatible list sizes");

    // Draw every rectangle in list
    Rectangle re;
    Iterator<Rectangle> i(tmpL);
    Iterator<Tensor5<bool> > thisCompatibleHypItr(compatibleHypotheses);
    Tensor5<bool> thisCompatibleHyp;

    //skip noHiddenVehicle case
    thisCompatibleHypItr(thisCompatibleHyp);

    while (i(re) && thisCompatibleHypItr(thisCompatibleHyp))
    {
        // ============ DRAW GREY HIDDEN AREA ============
        re.getBounds(bounds);

        const double width = bounds[0][1] - bounds[0][0];
        const double height = bounds[1][1] - bounds[1][0];

        const int widthPix = (int)round(SCALE * width);
        const int heightPix = (int)round(SCALE * height);
        const int x = (int)round((bounds[0][0] - cameraX) * SCALE);
        const int y = (int)round(Y_OFFSET - bounds[1][1] * SCALE);
        gdImageFilledRectangle(image, x, y, x + widthPix, y + heightPix, color);
        //gdImageRectangle(image, x, y, x + width, y + height, black);

        // ============ DRAW GRID =============

        // Number of dashed lines. -1 is for edges, see Predictor to see how hidden vehicle is placed in x,y
        const int nVerticalDashedLines = thisCompatibleHyp.Dim1 - 1;
        const int nHorizontalDashedLines = thisCompatibleHyp.Dim2 - 1;
        require(nVerticalDashedLines >= 0 && nHorizontalDashedLines >= 0, "Image::addAreaWithHiddenMap", "Negative number of dashed lines?");

        /* x axis ticks */
        const int nTicksX = 5;
        for (int xi = 1; xi <= nVerticalDashedLines; xi++)
            for (int t = 0; t < nTicksX; t++)
            {
                gdImageFilledRectangle(image,
                                       scaleXValue(bounds[0][0] + Predictor::getDeltaX() * xi),
                                       scaleYValue(bounds[1][1] - height / nTicksX * t) + 1,
                                       scaleXValue(bounds[0][0] + Predictor::getDeltaX() * xi) + tickWidth,
                                       scaleYValue(bounds[1][1] - height / nTicksX * t) + 1 + tickLength,
                                       black);
            }

        /* x axis ticks */
        const int nTicksY = 5;
        for (int yi = 1; yi <= nHorizontalDashedLines; yi++)
            for (int t = 0; t < nTicksY; t++)
            {
                gdImageFilledRectangle(image,
                                       scaleXValue(bounds[0][0] + width / nTicksY * t) + 1,
                                       scaleYValue(bounds[1][0] + Predictor::getDeltaY() * yi) + 1,
                                       scaleXValue(bounds[0][0] + width / nTicksY * t) + 1 + tickLength,
                                       scaleYValue(bounds[1][0] + Predictor::getDeltaY() * yi) + 1 + tickWidth,
                                       black);
            }

        // ========= DRAW HYPOTHESES AS CIRCLES ============
        for (int xi = 0; xi < thisCompatibleHyp.Dim1; xi++)
            for (int yi = 0; yi < thisCompatibleHyp.Dim2; yi++)
            {
                // first check if there is at least one hypothesis in the x,y point
                bool found = false;
                for (int thetai = 0; thetai < thisCompatibleHyp.Dim3; thetai++)
                {
                    for (int vi = 0; vi < thisCompatibleHyp.Dim4; vi++)
                    {
                        for (int desiredVi = 0; desiredVi < thisCompatibleHyp.Dim5; desiredVi++)
                        {
                            if (thisCompatibleHyp(xi, yi, thetai, vi, desiredVi) == true)
                            {
                                found = true;
                                break;
                            }
                        }

                        if (found)
                            break;
                    }
                    if (found)
                        break;
                }

                if (!found)
                    continue;

                // If it gets here, a valid hypothesis was found
                
                // Space location of hypothesis
                double xHyp;
                if (xi != thisCompatibleHyp.Dim1 - 1)
                    xHyp = bounds[0][0] + Predictor::getDeltaX() / 2.0 + xi * Predictor::getDeltaX();
                else //this case is to account for possible remainder in the ratio. If exact, doesn't change anything
                    xHyp = (bounds[0][0] + (xi*Predictor::getDeltaX() + bounds[0][1]))/2.0;

                double yHyp;
                if (yi != thisCompatibleHyp.Dim2 - 1)
                    yHyp = bounds[1][0] + Predictor::getDeltaY()/2.0 + yi * Predictor::getDeltaY();
                else //this case is to account for possible remainder in the ratio. If exact, doesn't change anything
                    yHyp = (bounds[1][0] + (yi*Predictor::getDeltaY() + bounds[1][1]))/2.0;



                gdImageFilledEllipse(hypImg, scaleXValue(xHyp), scaleYValue(yHyp), 7, 7, blue);
            }

    }

    gdImageColorTransparent(image, white);
    gdImageColorTransparent(hypImg, white);
    gdImageCopyMerge(frame, image, 0, 0, 0, 0, FRAME_W, FRAME_H, AREA_OPACITY);
    gdImageCopyMerge(frame, hypImg, 0, 0, 0, 0, FRAME_W, FRAME_H, 100);
}

int Image::scaleXValue(const double& x)
{
    return (int)round(SCALE * (x - cameraX));
}

int Image::scaleYValue(const double& y)
{
    return (int)round(Y_OFFSET - y * SCALE);
}