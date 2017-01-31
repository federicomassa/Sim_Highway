/*
 * DESIGN:      Cooperative Multi-Agent Systems Simulator
 *
 * FILE:        Rectangle.h
 * DESCRIPTION: Interface for the Rectangle class
 *
 * AUTHORS:     Marco Pellinacci <marco.pellinacci@alice.it>
 *              Gianni Valenti <posta@gianni.valenti.name>
 * DATE:        January 1, 2000
 */
 
#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "definition.h"
#include "EndLine.h"
#include "List.h"
#include "utility.h"
#include "Vector.h"

/*!
 * This class represents a cuboid in a N-dimensional space. Each edge has to be
 * parallel to the direction of a vector of the orthonormal vector basis.
 * The object is defined by a couple of double for each dimension of the space.
 * Each couple contains two coordinates and represents the bounds of an edge
 * -- the line segment where two faces meet -- along its direction.
 * Because two opposite faces of the N-dimensional cuboid are parallel, we only
 * need N segments to represent it.
 * The space dimension N is defined by the constant Q_DIM.
 */
class Rectangle
{
    /*!
     * \brief Class Area must be friend of this.
     */
    friend class Area;
    /*!
     * \brief Redefinition of operator <<.
     */
    friend ostream& operator<<(ostream& os, const Rectangle& r);
    /*!
     * \brief Redefinition of operator ==.
     *
     * This is the redefinition of operator ==.
     * Two rectangles are equals if all their bounds coincide.
     * @return True if the rectangles are equal, false otherwise.
     */
    friend bool operator==(const Rectangle& r1, const Rectangle& r2)
    {
        return r1.bounds == r2.bounds;
    }
    /*!
     * \brief Redefinition of operator != using operator ==.
     *
     * @return True if the rectangles aren't equal, false otherwise.
     */
    friend bool operator!=(const Rectangle& r1, const Rectangle& r2)
    {
        return !(r1 == r2);
    }
    /*!
     * \brief This function is a kind of redefinition of operator sizeof().
     */
    friend int size(Rectangle r)
    {
        return sizeof(r);
    }
    /*!
     * \brief A matrix of double representing rectangle's edges.
     */
    Vector<Vector<double, 2>, Q_DIM> bounds;

public:
    bool isDummy;
    /*!
     * \brief Default constructor.
     */
    Rectangle() { isDummy = true;}
    /*!
     * \brief Alternative constructor.
     *
     * This is an alternative constructor that receives as input a matrix of
     * double representing the rectangle's bounds along all directions and
     * initialize the object.
     * @param b matrix of doubles representing the rectangle's bounds.
     */
    explicit Rectangle(const Vector<Vector<double, 2>, Q_DIM>& b);
    /*!
     * \brief Destructor.
     */
    ~Rectangle() { }
    /*!
     * \brief Check if this rectangle contains a point.
     *
     * This method checks if this rectangle contains a point. A point in
     * the N-dimentional space is represent by an array of doubles.
     * @param point N-dimensional Vector of doubles.
     * @return True if the point belongs to the rectangle, false otherwise.
     */
    bool contains(const Vector<double, Q_DIM>& point) const;
    /*!
     * \brief Check if this rectangle contains another.
     *
     * This method checks if this rectangle contains another. Because each
     * rectangle is represented by an array of segments, a rectangle is said to
     * contain another if each segment of the second rectangle is a sub-segment
     * of the corresponding segment of the first one.
     * @param r Rectangle object.
     * @return True if the second rectangle is contained by the first one,
     * false otherwise.
     */
    bool contains(const Rectangle& r) const;
    /*!
     * \brief Check if this rectangle intersects or is adjacent to another.
     *
     * This method checks if this rectangle intersects or is adjacent to
     * another. An intersection is found if, for all the couple of
     * corresponding edges, their projection onto their direction have a not
     * null intersection. Even if some intersection collapse to a point the two
     * rectancles are adjacent and the method returns true.
     * @param r Rectangle object.
     * @return True if intersection or adjacence is found, false otherwise.
     */
    bool intersect(const Rectangle& r) const;
    /*!
     * \brief Check if this rectangle intersects another.
     *
     * This method checks if this rectangle intersects another. If the two
     * rectangles are adjacent the result is false.
     * @see intersect()
     * @param r Rectangle object.
     * @return True if intersection is found, false otherwise.
     */
    bool strictlyIntersect(const Rectangle& r) const;
    /*!
     * \brief Divide two overlapping rectangles into a list of non-overlapping
     *        slices.
     *
     * This method checks if there is a not null overlapping between two
     * rectangle -- the first is the object this method is applied to -- and
     * generates a list of non-overlapping slices representing
     * the union of the two objects.
     * @param r Rectangle object.
     * @param list Rectangle list where the method store the result.
     * @return True if the two rectangles overlap, false otherwise.
     */
    bool slice(const Rectangle& r, List<Rectangle>& list) const;
    /*!
     * \brief Check if this rectangle is aligned with another along a direction.
     *
     * This method checks if this rectangle is aligned with another one along
     * the direction of the n-th basis vector.
     * @param r Rectangle object.
     * @param n basis vector index.
     * @return True if the two rectangles are aligned, false otherwise.
     */
    bool aligned(const Rectangle& r, int n) const;
    /*!
     * \brief Weld this rectangle with another along a direction.
     *
     * This method weld this rectangle with another along the direction of the
     * n-th basis vector.
     * Note that this method doesn't check if the rectangles are really aligned
     * along the direction of the n-t basis vector; if they aren't the result
     * is not valid.
     * @param r Rectangle object.
     * @param n basis vector index.
     */
    void weld(const Rectangle& r, int n);
    /*!
     * \brief Return the rectangle bounds.
     *
     * @param b matrix of double -- filled by this method.
     */
    void getBounds(Vector<Vector<double, 2>, Q_DIM>& b) const { b = bounds; }
    /*!
     * \brief Return a measure of the rectangle.
     *
     * @return A double representing the measure.
     */
    double measure() const;
};

#endif
