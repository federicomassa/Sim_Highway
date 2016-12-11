
#ifndef OUTPUT_H
#define OUTPUT_H

#include <iomanip>
#include <fstream>
#include <sys/dir.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "definition.h"
#include "utility.h"

class Output
{
    static const char separator;
    
    int nCols;
    int currCol;
    ofstream of;
public:
    Output(string fn, int nc);
    ~Output() { of.close(); }
    void insert(int i);
    void insert(double d);
    void insert(string s);
};

#endif
