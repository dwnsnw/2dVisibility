# 2dVisibility
# 2dVisibility

This library is used with box2d, it can get box2dpolygon's vertices and construct a visibility polygon for a given viewportArea;

  Some important functions are as followed:

  1)void addSegment( const b2Vec2& p1, const b2Vec2& p2 )  
  add a segment which endpoints are p1 and p2, their order is not important  
  Arguments:  
  p1 - one endpoint of segment  
  p2 - another enpoint of segment
  
  2)void setviewerLocation( const b2Vec2& viewerLocation )  
  set location of light  
  Arugment:  
  viewerPosition - the location of viewer

  3)void converToSegments( const b2Vec2* polygon, int verticesCount )  
  change the polygon's vertices to segments and add into the VisibilityPolygonObject  
  Arugment:  
  polygon - polygon vectices array  
  verticesCount - number of vertices

  4)void sweep()  
  compute the polygon using the object data, so you should add all data that you need before call this function
  
  5)void generatePolygon( b2World* m_world, b2Vec2 viewerLoc, const b2Vec2& viewportMinCorner, const b2Vec2& viewportMaxCorner );  
  this function is uisng with box2d testbed, you can just using this function to generate polygon data and get it for output array, or you can to know how to using other function to compute polygon from it;  
  Arugment:  
  m_world - your b2world  
  viewerLoc - your viewer location  
  viewportMinCorner - the minmum viewport area position  
  viewportMaxCorner - the maxmum viewport area position
