#ifndef TRUST_H
#define TRUST_H

#include <iostream>

using namespace std;

class Trust
{
public:
    int agentID; /* agent ID */
    double global; /* global trust value */
    double local; /* local trust value */

    /* constructor */
    Trust(int a = -1, double g = 0, double l = 0)
    {
        init(a, g, l);
    }
    /* destructor */
    ~Trust() { }
    /* initialization */
    void init(int a, double g, double l)
    {
        agentID = a;
        global = g;
        local = l;
    }
};

ostream& operator<<(ostream&, const Trust&);
bool operator==(const Trust&, const Trust&);
bool operator!=(const Trust&, const Trust&);

#endif
