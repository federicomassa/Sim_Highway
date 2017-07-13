/*
 * DESIGN:      Cooperative Multi-Agent Systems Simulator
 *
 * FILE:        Area.h
 * DESCRIPTION: interface for the Area class
 *
 * AUTHORS:     Marco Pellinacci <marco.pellinacci@alice.it>
 *              Gianni Valenti <posta@gianni.valenti.name>
 * DATE:        January 1, 2000
 */

#ifndef AREA_H
#define AREA_H

#include "Logger.h"
#include "Rectangle.h"

extern Logger LOG;

/*!
 * This class represents a region of an N-dimensional space, composed by a list
 * of Rectangle objects.
 */
class Area
{
    /*!
     * \brief Redefinition of operator <<.
     */
    friend std::ostream& operator<<(std::ostream&, const Area&);
    /*!
     * \brief Redefinition of operator * for intersection.
     */
    friend Area operator*(const Area& a1, const Area& a2);
    /*!
     * \brief Redefinition of operator + for union.
     */
    friend Area operator+(const Area& a1, const Area& a2);
    /*!
     * \brief Redefinition of operator - for subtraction.
     */
    friend Area operator-(const Area& a1, const Area& a2);
    /*!
     * \brief This function is a kind of redefinition of operator sizeof().
     */
    friend int size(Area a)
    {
        return size(a.rectList);
    }
    /*!
     * \brief A list of rectangles.
     */
    List<Rectangle> rectList;
    /*!
     * \brief Add a rectangle to the list.
     *
     * This method adds a rectangle to the list without checking for
     * overlapping. Method simplify() can be used to calculate a new equivalent
     * non-overlapping rectangle list.
     * @param r rectangle to add.
     */
    bool addRect(const Rectangle& r);
    /*!
     * \brief Remove a rectangle from the list.
     *
     * @param r rectangle to remove.
     */
    void delRect(const Rectangle& r) { rectList.delInfo(r); }
    /*!
     * \brief If in the list there are two rectangles one inside the other, the
     *        smaller one is deleted.
     */
    void simplifyByContain();
    /*!
     * \brief If in the list there are two overlapping rectangles they are
     *        substituted by a new equivalent non-overlapping rectangle set.
     */
    void expandByIntersect();
    /*!
     * \brief If in the list there are two rectangles aligned along a direction
     *        they are substituted by a single bigger rectangle.
     */
    void simplifyByAlign();
    /*!
     * \brief Delete the rectangles having null measure.
     */
    void delNullRectangles();
public:
    /*!
     * \brief Default constructor.
     */
    Area() { }
    /*!
     * \brief Destructor.
     */
    ~Area() { }
    /*!
     * \brief Redefinition of operator *= for intersection.
     */
    Area& operator*=(const Area& a2);
    /*!
     * \brief Redefinition of operator += for union.
     */
    Area& operator+=(const Area& a2);
    /*!
     * \brief Redefinition of operator -= for subtraction.
     */
    Area& operator-=(const Area& a2);

    bool operator==(const Area& a2) const;
    bool operator!=(const Area& a2) const
    {
        return !((*this) == a2);
    }
    /*!
     * \brief Add a rectangle to the list.
     *
     * This method adds a rectangle to the list without checking for
     * overlapping. The simplify() method can be used to generate a new
     * equivalent non-overlapping rectangle list.
     * @param b matrix of double representing the rectangle bounds.
     * @return True if the rectangle wasn't in the list, false otherwise.
     */
    bool addRect(const Vector<Vector<double, 2>, Q_DIM> b)
    {
        Rectangle r(b);
        return addRect(r);
    }
    /*!
     * \brief Check if this area is empty.
     *
     * @return True if this area is empty, false otherwise.
     */
    bool isEmpty() const { return rectList.isEmpty(); }
    /*!
     * \brief Empty the rectangle list.
     */
    void purge() { rectList.purge(); }
    /*!
     * \brief Check if this area contains a point.
     *
     * This method checks if this area contains a point. The result is true if
     * at least one of the rectangle in the list contains the point. A point in
     * the N-dimentional space is represent by an array of doubles.
     * @param point N-dimensional array of doubles.
     * @return True if the point belongs to this area, false otherwise.
     */
    bool contains(const Vector<double, Q_DIM>& point) const;
    /*!
     * \brief Check if this area contains a rectangle.
     *
     * This method checks if this area contains a rectangle. The result is true
     * if at least one of the rectangle in the list contains the rectangle.
     * @param r rectangle.
     * @return True if the rectangle is contained by this area, false otherwise.
     */
    bool contains(const Rectangle& r) const;
    /*!
     * \brief Simplify the rectangle list.
     */
    void simplify();
    /*!
     * \brief Return the rectangle list.
     */
    void getRectList(List<Rectangle>& l) const { l = rectList; }
    /*!
     * \brief Return a measure of the area.
     *
     * @return The sum of rectangles measures.
     */
    double measure() const;
};

#endif
