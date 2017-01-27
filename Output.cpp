
#include "Output.h"

const char Output::separator = '\t';

using namespace std;

Output::Output(string fn, int nc)
{
    /* create output dir */
    mkdir(OUTPUT_DIR, S_IRWXU | S_IRGRP | S_IROTH);
    
    string fileName = (string)OUTPUT_DIR + "/Out-" + fn + ".txt";
    
    /* open log stream */
    of.open(fileName.c_str(), ios_base::app);

    /* error handling */
    if(!of.is_open())
        error("Output", "Cannot open file " + fileName);
    
    nCols = nc;
    currCol = 0;
}

void Output::insert(int i)
{
    /* error handling */
    if(!of.is_open())
        error("Output", "stream is closed.");

    /* set numbers representation */
    of << fixed << setprecision(0);

    of << i;
    currCol++;
    
    if(currCol < nCols)
        of << separator;
    else
    {
        currCol = 0;
        of << endl;
    }
}

void Output::insert(double d)
{
    /* error handling */
    if(!of.is_open())
        error("Output", "stream is closed.");
    
    /* set numbers representation */
    of << fixed << setprecision(5);
    
    of << d;
    currCol++;
    
    if(currCol < nCols)
        of << separator;
    else
    {
        currCol = 0;
        of << endl;
    }
}

void Output::insert(string s)
{
    /* error handling */
    if(!of.is_open())
        error("Output", "stream is closed.");
    
    of << s;
    currCol++;
    
    if(currCol < nCols)
        of << separator;
    else
    {
        currCol = 0;
        of << endl;
    }
}

