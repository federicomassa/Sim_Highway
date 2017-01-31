/*
 * DESIGN:      Cooperative Multi-Agent Systems Simulator
 *
 * FILE:        Rectangle.cpp
 * DESCRIPTION: Implementation of the Rectangle class
 *
 * AUTHORS:     Marco Pellinacci <marco.pellinacci@alice.it>
 *              Gianni Valenti <posta@gianni.valenti.name>
 * DATE:        January 1, 2000
 */

#include "Rectangle.h"

Rectangle::Rectangle(const Vector<Vector<double, 2>, Q_DIM>& b)
{
  isDummy = false;
    /* error handling */
    for(int i = 0; i < Q_DIM; i++)
        if(b[i][0] > b[i][1])
            error("Rectangle", "bounds order not valid");

    bounds = b;
}

bool Rectangle::contains(const Vector<double, Q_DIM>& point) const
{
    for(int i = 0; i < Q_DIM; i++)
        if(point[i] < bounds[i][0] || point[i] >= bounds[i][1])
            return false;
    
    return true;
}

bool Rectangle::contains(const Rectangle& r) const
{
    for(int i = 0; i < Q_DIM; i++)
        if(r.bounds[i][0] < bounds[i][0] || r.bounds[i][1] > bounds[i][1])
            return false;
    
    return true;
}

bool Rectangle::strictlyIntersect(const Rectangle& r) const
{
    for(int i = 0; i < Q_DIM; i++)
        if(bounds[i][0] >= r.bounds[i][1] || bounds[i][1] <= r.bounds[i][0])
            return false;

    return true;
}

bool Rectangle::intersect(const Rectangle& r) const
{
    for(int i = 0; i < Q_DIM; i++)
        if(bounds[i][0] > r.bounds[i][1] || bounds[i][1] < r.bounds[i][0])
            return false;

    return true;
}

bool Rectangle::aligned(const Rectangle& r, int n) const
{
    /* error handling */
    if(n < 0 || n > Q_DIM)
        error("Rectangle", "dimension n greater than space dimension");

    for(int i = 0; i < Q_DIM; i++)
    {
        if(n == i)
            continue;
        if(bounds[i] != r.bounds[i])
            return false;
    }
    
    return true;
}

bool Rectangle::slice(const Rectangle& r, List<Rectangle>& list) const
{
    /* if the rectangles don't have an intersection return an empty area */
    if(!intersect(r))
        return false;
    
    /* empty destination list */
    list.purge();
    
    /* computing intersection */
    Vector<Vector<double, 4>, Q_DIM> vertex;
    for(int i = 0; i < Q_DIM; i++){
        vertex[i][0] = bounds[i][0];
        vertex[i][1] = bounds[i][1];
        vertex[i][2] = r.bounds[i][0];
        vertex[i][3] = r.bounds[i][1];
        vertex[i].sort();
    }
    
    /* building 3^Q_DIM rectangles */
    Vector<Vector<double, 2>, Q_DIM> bounds;
    int counter[Q_DIM];
    for(int i = 0; i < Q_DIM; i++)
        counter[i] = 0;
    bool exitCondition = false;
    while(!exitCondition)
    {
        /* creating rectangle */
        for(int i = 0; i < Q_DIM; i++)
        {
            bounds[i][0] = vertex[i][counter[i]];
            bounds[i][1] = vertex[i][counter[i] + 1];
        }
        Rectangle newRect(bounds);
        /* add rectangle to the new area */
        if(contains(newRect) || r.contains(newRect))
            list.insHead(newRect);
        /* checking exit condition and updating dimension counters */
        counter[Q_DIM - 1]++;
        for(int i = Q_DIM - 1; i >= 0; i--)
        {
            if(counter[i] == 3)
            {
                if(i == 0)
                {
                    exitCondition = true;
                    break;
                }
                else
                {
                    counter[i] = 0;
                    counter[i - 1]++;
                }
            }
        }
    } /* while(!exitCondition) */

    return true;
}

void Rectangle::weld(const Rectangle& r, int n)
{
  isDummy = false;
    /* error handling */
    if(n < 0 || n > Q_DIM)
        error("Rectangle", "dimension n greater than space dimension");

    /* computing intersection */
    Vector<double, 4> vertex;
    vertex[0] = bounds[n][0];
    vertex[1] = bounds[n][1];
    vertex[2] = r.bounds[n][0];
    vertex[3] = r.bounds[n][1];
    vertex.sort();
    bounds[n][0] = vertex[0];
    bounds[n][1] = vertex[3];
}

double Rectangle::measure() const
{
    double m = 1;
    for(int i = 0; i < Q_DIM; i++)
    {
        m *= bounds[i][1] - bounds[i][0];
    }
    return m;
}

ostream& operator<<(ostream& os, const Rectangle& r)
{
    os << "Rectangle ( ";
    for(int i = 0; i < Q_DIM; i++)
    {
        /* check if bounds collapse in a point */
        if(r.bounds[i][0] == r.bounds[i][1])
        {
            os << 'q' << (i + 1) << " = ";
            os << r.bounds[i][0];
        }
        else
        {
            os << r.bounds[i][0] << " < ";
            os << 'q' << (i + 1) << " < " << r.bounds[i][1];
        }
        if (i < Q_DIM - 1)
            os << "; ";
    }
    os << " )";
    
    return os;
}
