//
//  VisibilityPolygon.cpp
//  Box2D
//
//  Created by DawnSnow on 16/8/19.
//
//

#include "VisibilityPolygon.h"
#include<math.h>
#include<iostream>

using std::cout;
using std::endl;

VisibilityPolygon::VisibilityPolygon()
{
    initData();
}

void VisibilityPolygon::initData()
{
    segmentsCount = 0;
    outputsCount = 0;
    
    for ( int i = 0; i < maxSegmentsCount; i++ )
    {
        segments[ i ] = Segment();
        output[ 2 * i ] = segments[ i ].point1.point;
        output[ 2 * i + 1 ] = segments[ i ].point2.point;
    }
    
    viewportMaxCorner = b2Vec2( 0, 0 );
    viewportMinCorner = b2Vec2( 0, 0 );
}

int VisibilityPolygon::getOutputCount() const
{
    return outputsCount;
}

void VisibilityPolygon::sweep()
{
    EndPoint *points = (EndPoint * )b2Alloc( segmentsCount * 2 * sizeof( EndPoint ) );
    int pointsCount = 0;
    for ( int i = 0; i < segmentsCount; i++ )
    {
        points[ pointsCount ] = segments[ i ].point1;
        ++pointsCount;
        points[ pointsCount ] = segments[ i ].point2;
        ++pointsCount;
    }
    sortPoints( points, pointsCount );
    float32 beginAngle = 0.0;
    for ( int pass = 0; pass <= 2; ++pass )
    {
        for ( int i = 0; i < pointsCount; ++i )
        {
            EndPoint p = points[ i ];
            Segment *currentOld = open.empty()? NULL : &*open.begin();
            
            if ( p.begin )
            {
                list<Segment>::iterator node = open.begin();
                while ( node != open.end() && segmentInFrontOf( *p.segment, *node, lightPosition ) )
                    node++;
                if ( node == open.end() )
                {
                    open.push_back( *p.segment );
                }
                else
                {
                    open.insert( node, *p.segment );
                }
            }
            else
            {
                for ( list<Segment>::iterator node = open.begin(); node != open.end(); ++node )
                {
                    if ( (Segment)*node == *p.segment )
                    {
                        open.erase( node );
                        break;
                    }
                }
            }
            Segment *currentNew = open.empty()? NULL : &*open.begin();
            if ( currentNew != currentOld )
            {
                if ( pass == 1 )
                {
                    addTriangle(beginAngle, p.angle, *currentOld );
                }
                beginAngle = p.angle;
            }
        }

    }
}

void VisibilityPolygon::convertToSegments(const b2Vec2 *polygon, int verticesCount )
{
    for ( int i = 0; i < verticesCount - 1; i++ )
    {
        addSegment( polygon[ i ], polygon[ i + 1 ] );
    }
    addSegment( polygon[ verticesCount - 1 ], polygon[ 0 ] );
}

void VisibilityPolygon::addTriangle( float32 angle1, float32 angle2, Segment segment )
{
    b2Vec2 p1 = lightPosition;
    b2Vec2 p2( lightPosition.x + cosf( angle1 ), lightPosition.y + sinf( angle1 ) );
    b2Vec2 p3;
    b2Vec2 p4;
    
    p3.x = segment.point1.x;
    p3.y = segment.point1.y;
    p4.x = segment.point2.x;
    p4.y = segment.point2.y;
    
    b2Vec2 pBegin = lineIntersection( p3, p4, p1, p2 );
    
    p2.x = lightPosition.x + cosf( angle2 );
    p2.y = lightPosition.y + sinf( angle2 );
    
    b2Vec2 pEnd = lineIntersection( p3, p4, p1, p2 );
    
    output[ outputsCount ] = pBegin;
    outputsCount++;
    output[ outputsCount ] = pEnd;
    outputsCount++;
}

void VisibilityPolygon::loadEdgeOfViewPort( const b2Vec2 &viewportMinCorner, const b2Vec2 &viewportMaxCorner )
{
    b2Vec2 rightUp( viewportMaxCorner.x, viewportMaxCorner.y );
    b2Vec2 rightDown( viewportMaxCorner.x, viewportMinCorner.y );
    b2Vec2 leftUp( viewportMinCorner.x, viewportMaxCorner.y );
    b2Vec2 leftDown( viewportMinCorner.x, viewportMinCorner.y );
    
    addEdgeSegment( rightUp, leftUp );
    addEdgeSegment( leftUp, leftDown );
    addEdgeSegment( leftDown, rightDown );
    addEdgeSegment( rightDown, rightUp );
}

void VisibilityPolygon::addSegment( const b2Vec2& p1, const b2Vec2& p2, int start )
{
    if ( segmentsCount > maxSegmentsCount )
        return;
    if ( ( p1.x >= viewportMaxCorner.x || p1.y >= viewportMaxCorner.y ) && ( p2.x >= viewportMaxCorner.x || p2.y >= viewportMaxCorner.y ) )
        return;
    if ( ( p1.x <= viewportMinCorner.x || p1.y <= viewportMinCorner.y ) && ( p2.x <= viewportMinCorner.x || p2.y <= viewportMinCorner.y ) )
        return;
    EndPoint point1( p1.x, p1.y );
    EndPoint point2( p2.x, p2.y );
    
    point1.visualize = true;
    point2.visualize = false;
    
    Segment segment( point1, point2 );
    trySegmentsIntersect( segment, start );
}

void VisibilityPolygon::addEdgeSegment( const b2Vec2& p1, const b2Vec2& p2 )
{
    if ( segmentsCount > maxSegmentsCount )
        return;
    EndPoint point1( p1.x, p1.y );
    EndPoint point2( p2.x, p2.y );
    
    point1.visualize = true;
    point2.visualize = false;
    
    Segment segment( point1, point2 );
    trySegmentsIntersect( segment, 0 );
}

bool VisibilityPolygon::trySegmentsIntersect(const Segment &s1, int start )
{
    for ( int i = start; i < segmentsCount; ++i )
    {
        Segment s2 = segments[ i ];
        bool A1 = leftOf( s1, interpolate( s2.point1.point, s2.point2.point, 0.01 ) );
        bool A2 = leftOf( s1, interpolate( s2.point2.point, s2.point1.point, 0.01 ) );
        bool B1 = leftOf( s2, interpolate( s1.point1.point, s1.point2.point, 0.01 ) );
        bool B2 = leftOf( s2, interpolate( s1.point2.point, s1.point1.point, 0.01 ) );
        if ( A1 != A2 && B1 != B2 )
        {
            b2Vec2 eplision( 0.01, 0.01 );
            b2Vec2 intersectPoint = lineIntersection( s1.point1.point, s1.point2.point, s2.point1.point, s2.point2.point );
            addSegment( s1.point1.point, intersectPoint, i );
            addSegment( intersectPoint, s1.point2.point, i );
            return false;
        }
    }
    segments[ segmentsCount ] = s1;
    segments[ segmentsCount ].point1.segment = &segments[segmentsCount];
    segments[ segmentsCount ].point2.segment = &segments[segmentsCount];
    segmentsCount++;
    return true;
}

void VisibilityPolygon::setLightLocation( const b2Vec2& position )
{
    lightPosition = position;
    
    for( int i = 0; i < segmentsCount; ++i )
    {
        segments[ i ].point1.angle = atan2f( segments[ i ].point1.y - lightPosition.y, segments[ i ].point1.x - lightPosition.x );
        segments[ i ].point2.angle = atan2f( segments[ i ].point2.y - lightPosition.y, segments[ i ].point2.x - lightPosition.x );
        
        float32 dAngle = segments[ i ].point2.angle - segments[ i ].point1.angle;
        
        if ( dAngle <= -M_PI )
        {
            dAngle += 2 * M_PI;
        }
        if ( dAngle > M_PI )
        {
            dAngle -= 2 * M_PI;
        }
        
        segments[ i ].point1.begin = ( dAngle > 0 );
        segments[ i ].point2.begin = !segments[ i ].point1.begin;
    }
}

int VisibilityPolygon::endPointCompare( const EndPoint &p1, const EndPoint &p2 )
{
    if ( p1.angle > p2.angle ) return 1;
    if ( p1.angle < p2.angle ) return -1;
    
    if ( !p1.begin && p2.begin ) return 1;
    if ( p1.begin && !p2.begin ) return -1;
    
    return 0;
}

void VisibilityPolygon::sortPoints( EndPoint *points, int pointsCount )
{
    for ( int i = 1; i < pointsCount; ++i )
    {
        for ( int j = i - 1; j >= 0; --j )
        {
            if ( endPointCompare( points[ j + 1 ], points[ j ] ) == -1 )
            {
                EndPoint temp = points[ j + 1 ];
                points[ j + 1 ] = points[ j ];
                points[ j ] = temp;
                continue;
            }
            break;
        }
    }
}

b2Vec2 VisibilityPolygon::interpolate( const b2Vec2 &p1, const b2Vec2 &p2, const float32 &f )
{
    return b2Vec2( p1.x * ( 1 - f ) + p2.x * f , p1.y * ( 1 - f ) + p2.y * f );
}

bool VisibilityPolygon::segmentInFrontOf( const Segment &s1, const Segment &s2, b2Vec2 relativeTo )
{
    bool A1 = leftOf( s1, interpolate( s2.point1.point, s2.point2.point, 0.01 ) );
    bool A2 = leftOf( s1, interpolate( s2.point2.point, s2.point1.point, 0.01 ) );
    bool A3 = leftOf( s1, relativeTo );
    bool B1 = leftOf( s2, interpolate( s1.point1.point, s1.point2.point, 0.01 ) );
    bool B2 = leftOf( s2, interpolate( s1.point2.point, s1.point1.point, 0.01 ) );
    bool B3 = leftOf( s2, relativeTo );
    
    if ( B1 == B2 && B2 != B3 ) return true;
    if ( A1 == A2 && A2 == A3 ) return true;
    if ( A1 == A2 && A2 != A3 ) return false;
    if ( B1 == B2 && B2 == B3 ) return false;
    
    return false;
}

b2Vec2 VisibilityPolygon::lineIntersection( const b2Vec2 &p1, const b2Vec2 &p2, const b2Vec2 &p3, const b2Vec2 &p4 )
{
    float32 s = ((p4.x - p3.x) * (p1.y - p3.y) - (p4.y - p3.y) * (p1.x - p3.x)) / ((p4.y - p3.y) * (p2.x - p1.x) - (p4.x - p3.x) * (p2.y - p1.y));
    return b2Vec2(p1.x + s * (p2.x - p1.x), p1.y + s * (p2.y - p1.y));
}



