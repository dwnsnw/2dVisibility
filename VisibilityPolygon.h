//
//  VisibilityPolygon.h
//  Box2D
//
//  Created by DawnSnow on 16/8/19.
//
//

#ifndef __Box2D__VisibilityPolygon__
#define __Box2D__VisibilityPolygon__

#include <stdio.h>
#include "b2Math.h"
#include <list>
#include <vector>
#include "b2WorldCallbacks.h"
#include "b2Fixture.h"
#include "b2Contact.h"
#include "b2PolygonShape.h"
#include "b2World.h"

using std::list;
using std::vector;

struct EndPoint;
struct Segment;

struct EndPoint
{
public:
    Segment* segment = nullptr;
    float32 angle = 0.0;
    bool begin = false;
    bool visualize = false;
    b2Vec2 point;
    float32 x;
    float32 y;
    EndPoint( const float32& xin, const float32& yin)
    {
        point.x = xin;
        point.y = yin;
        x = xin;
        y = yin;
    }
    EndPoint()
    {
        point.x = 0;
        point.y = 0;
        x = 0;
        y = 0;
    }
    bool operator== ( const EndPoint p )
    {
        if ( this->x == p.x && this->y == p.y && this->segment == p.segment && this->begin == p.begin  )
        {
            return true;
        }
        return false;
    }
};

struct Segment
{
public:
    EndPoint point1;
    EndPoint point2;
    float32 length;
    Segment( EndPoint p1, EndPoint p2 )
    {
        point1 = p1;
        point2 = p2;
        point1.segment = NULL;
        point2.segment = NULL;
        float32 dx = point2.x - point1.x;
        float32 dy = point2.y - point1.y;
        length = sqrtf( dx*dx + dy*dy );
    }
    Segment()
    {
        EndPoint p1;
        EndPoint p2;
        point1.segment = NULL;
        point2.segment = NULL;
        length = 0;
    }
    bool operator== ( const Segment s )
    {
        if ( this->point1 == s.point1 && this->point2 == s.point2 )
        {
            return true;
        }
        return false;
    }
};

class ObjectQueryCallback : public b2QueryCallback
{
public:
    vector<b2Fixture*> polygon;

    bool ReportFixture( b2Fixture* lightenedFixture )
    {
        b2Shape* shape = lightenedFixture->GetShape();
        if ( shape->m_type == b2Shape::e_polygon )
        {
            polygon.push_back( lightenedFixture );
        }
        return true;
    }
};


class VisibilityPolygon
{
private:
    const static int maxSegmentsCount = 1000;
    Segment segments[ maxSegmentsCount ];
    list<Segment> open;
    
    b2Vec2 lightPosition;
    b2Vec2 viewportMaxCorner;
    b2Vec2 viewportMinCorner;
    int segmentsCount;
    int outputsCount;
    void loadEdgeOfViewPort( const b2Vec2& viewportMinCorner, const b2Vec2& viewportMaxCorner );
    int endPointCompare( const EndPoint& p1, const EndPoint& p2 );
    bool segmentInFrontOf( const Segment& s1, const Segment& s2, b2Vec2 relativeTo );
    void sortPoints( EndPoint* points, int pointsCount );
    bool inline leftOf( const Segment& s, const b2Vec2& p );
    b2Vec2 interpolate( const b2Vec2& p1, const b2Vec2& p2, const float32& f );
    b2Vec2 lineIntersection(const b2Vec2& p1, const b2Vec2& p2, const b2Vec2& p3, const b2Vec2& p4 );
    bool trySegmentsIntersect( const Segment& s, int start = 0 );
    void addTriangle( float32 angle1, float32 angle2, Segment segment );
    

public:
    b2Vec2 output[ 2 * maxSegmentsCount ];
    VisibilityPolygon();
    void initData();
    void addEdgeSegment( const b2Vec2& p1, const b2Vec2& p2 );
    void setLightLocation( const b2Vec2& lightPosition );
    void loadMapInViewPort( const Segment* segments, const int& count, const b2Vec2& viewportMinCorner, const b2Vec2& viewportMaxCorner );
    void addSegment( const b2Vec2& p1, const b2Vec2& p2,int start = 0 );
    void sweep();
    void convertToSegments( const b2Vec2* polygon, int verticesCount );
    void generatePolygon( b2World* m_world, b2Vec2 lightPos, const b2Vec2& viewportMinCorner, const b2Vec2& viewportMaxCorner );
    int getOutputCount() const;
};

bool inline VisibilityPolygon::leftOf( const Segment &s, const b2Vec2 &p )
{
    float32 cross = ( s.point2.x - s.point1.x ) * ( p.y - s.point1.y ) - ( s.point2.y - s.point1.y ) * ( p.x - s.point1.x );
    
    return cross < 0;
}
#endif /* defined(__Box2D__VisibilityPolygon__) */
