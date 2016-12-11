
#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <cmath>
#include <iostream>

using namespace std;

class ProgressBar
{
    static int rollIndex;
    bool backspaces;
    int current;
    int total;
    friend ostream& operator<<(ostream& os, const ProgressBar& p);
public:
    ProgressBar();
    ProgressBar(int c, int t);
};

#endif
