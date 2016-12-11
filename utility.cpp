/**
 * DESIGN:      Reputation/Trust System in Cooperative Multi-Agent Systems
 *
 * FILE:        Channel.h
 * DESCRIPTION: interface for the Channel class
 *
 * AUTHOR:      Marco Pellinacci
 * DATE:        June 9, 2007
 */

#include "utility.h"
#include <cstdlib>

/**
 * this function gives information about a specific error and it terminates
 * the program
 */
void error(const string className, const string msg)
{
    cerr << endl << "ERROR - " << className << " : " << msg << endl;
    exit(1);
}

/**
 * this function is called by set_new_handler()
 */
void exaustedMem()
{
    cerr << endl << "Exausted memory!" << endl;
    exit(2);
}

/**
 * this function returns the sign (+1 or -1) of a double
 */
int sign(const double x)
{
    if(x >= 0)
        return 1;
    else
        return -1;
}

string toString(int i, int pad)
{
    stringstream ss;
    if(pad)
    {
        ss.fill('0');
        ss.width(pad);
    }
    string str;
    ss << i;
    ss >> str;
    return str;
}
