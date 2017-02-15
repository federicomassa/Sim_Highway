
#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <cmath>
#include <iostream>


class ProgressBar
{
    static int rollIndex;
    bool backspaces;
    int current;
    int total;
    friend std::ostream& operator<<(std::ostream& os, const ProgressBar& p);
public:
    ProgressBar();
    ProgressBar(int c, int t);
};

#endif
