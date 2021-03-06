/*
 * DESIGN:      Cooperative Multi-Agent Systems Simulator
 *
 * FILE:        Area.cpp
 * DESCRIPTION: implementation of the Area class
 *
 * AUTHORS:     Marco Pellinacci <marco.pellinacci@alice.it>
 *              Gianni Valenti <posta@gianni.valenti.name>
 * DATE:        January 1, 2000
 */

#include "Area.h"
#include "State.h"

using namespace std;

bool Area::addRect(const Rectangle& r)
{
    return rectList.insHead(r, true);
}

/**
 * this method checks if the area contains the point
 */
bool Area::contains(const Rectangle& r2) const
{
    Iterator<Rectangle> rectIt(rectList);
    Rectangle r1;
    while (rectIt(r1))
    {
        if (r1.contains(r2))
            return true;
    }

    return false;
}

bool Area::contains(const Vector<double, Q_DIM>& point) const
{
    Iterator<Rectangle> rectIt(rectList);
    Rectangle r;
    while (rectIt(r))
    {
        if (r.contains(point))
            return true;
    }

    return false;
}

bool Area::contains(const State& q) const
{
    Vector<double, Q_DIM> point;
    point[0] = q.x;
    point[1] = q.y;

    return contains(point);
}


void Area::simplifyByContain()
{
    /* create a new rectangle list */
    List<Rectangle> newList;
    Rectangle r1;
    while (rectList.extrHead(r1))
    {
        bool insert = true;
        Iterator<Rectangle> rectIt2(newList);
        Rectangle r2;
        while (rectIt2(r2))
        {
            if (r1.contains(r2))
                /* if r1 contains a rectangle r2 in the new list, r2 has to be
                    deleted */
                newList.delInfo(r2);
            else if (r2.contains(r1))
            {
                /* if the new list has a rectangle containing r1, it has not to
                    be inserted in the new list */
                insert = false;
                break;
            }
        }
        if (insert)
            newList.insHead(r1);
    }
    rectList = newList;
}

void Area::expandByIntersect()
{
    bool exitCondition = false;
    while (!exitCondition)
    {
        exitCondition = true;
        Rectangle r1, r2;
        Iterator<Rectangle> it1(rectList);
        while (it1(r1))
        {
            Iterator<Rectangle> it2 = it1;
            while (it2(r2))
            {
                if (r1.strictlyIntersect(r2))
                {
                    List<Rectangle> tmpList;
                    r1.slice(r2, tmpList);
                    if (!tmpList.belongs(r1))
                        rectList.delInfo(r1);
                    if (!tmpList.belongs(r2))
                        rectList.delInfo(r2);
                    if (rectList.join(tmpList))
                    {
                        exitCondition = false;
                        break;
                    }
                }
            }
            if (!exitCondition)
                break;
        }
    }
}

void Area::simplifyByAlign()
{
    for (int i = 0; i < Q_DIM; i++)
    {
        /* create a new rectangle list */
        List<Rectangle> newList;
        Rectangle r1;
        while (rectList.extrHead(r1))
        {
            bool insert = true;
            Iterator<Rectangle> rectIt2(newList);
            Rectangle r2;
            while (rectIt2(r2))
            {
                /* check if the newList contains a rectangle aligned with r1
                    alogn the i-th dimension */
                if (r1.aligned(r2, i) && r1.intersect(r2))
                {
                    /* merge r1 with r2 among i-th dimension */
                    r1.weld(r2, i);

                    /* remove r2 and insert r1 */
                    newList.delInfo(r2);
                    newList.insHead(r1);

                    insert = false;
                    break;
                }
            }
            if (insert)
                newList.insHead(r1);
        }
        rectList = newList;
    }
}

void Area::delNullRectangles()
{
    Iterator<Rectangle> i(rectList);
    Rectangle r;
    while (i(r))
    {
        if (r.measure() == 0)
            rectList.delInfo(r);
    }
}

void Area::simplify()
{
    delNullRectangles();
    simplifyByAlign();
    simplifyByContain();
    expandByIntersect();
    delNullRectangles();
    simplifyByAlign();
    simplifyByContain();
}

double Area::measure() const
{
    double mu = 0;

    Iterator<Rectangle> ri(rectList);
    Rectangle r;
    while (ri(r))
        mu += r.measure();

    return mu;
}

Area& Area::operator*=(const Area& a2)
{
    if (this == &a2)
        return *this;

    Area tmpArea = *this;
    rectList.join(a2.rectList);

    expandByIntersect();

    Rectangle tmpR;
    Iterator<Rectangle> rectIt(rectList);
    while (rectIt(tmpR))
        if (!tmpArea.contains(tmpR) || !a2.contains(tmpR))
            delRect(tmpR);

    delNullRectangles();
    simplifyByAlign();
    simplifyByContain();

    return *this;
}

Area& Area::operator+=(const Area& a2)
{
    if (this == &a2)
        return *this;

    rectList.append(a2.rectList);
    simplify();

    return *this;
}

Area& Area::operator-=(const Area& a2)
{
    if (this == &a2)
    {
        rectList.purge();
        return *this;
    }

    Area tmpArea = *this;

    rectList.append(a2.rectList);
    expandByIntersect();

    Rectangle tmpR;
    Iterator<Rectangle> rectIt(rectList);
    while (rectIt(tmpR))
        if (!tmpArea.contains(tmpR) || a2.contains(tmpR))
            delRect(tmpR);

    delNullRectangles();
    simplifyByAlign();
    simplifyByContain();

    return *this;
}

Area operator*(const Area& a1, const Area& a2)
{
    Area newArea = a1;
    return newArea *= a2;
}

Area operator+(const Area& a1, const Area& a2)
{
    Area newArea = a1;
    return newArea += a2;
}

Area operator-(const Area& a1, const Area& a2)
{
    Area newArea = a1;
    return newArea -= a2;
}

ostream& operator<<(ostream& os, const Area& a)
{
    os << "Area {" << EndLine(EndLine::INC);
    os << "Rectangles: " << a.rectList << EndLine(EndLine::DEC);
    os << '}';

    return os;
}

bool Area::operator==(const Area& a2) const
{
    Iterator<Rectangle> iRect(rectList);
    Rectangle r;

    Iterator<Rectangle> iRect2(a2.rectList);
    Rectangle r2;

    if (rectList.count() != a2.rectList.count())
        return false;

    while (iRect(r) && iRect2(r2))
    {
        if (r != r2)
            return false;
    }

    return true;
}