/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/Coordinates.h"
#include "openocl/ui/GraphicsBase.h"
#include "openocl/ui/Region.h"

#include <openocl/base/Math.h>
#include <openocl/base/Runtime.h>
#include <openocl/base/String.h>
#include <openocl/imaging/Color.h>
#include <openocl/imaging/Geometry.h>
#include <openocl/imaging/Point.h>
#include <openocl/imaging/TextExtents.h>
#include <openocl/util/Sequence.h>
#include <openocl/util/Stack.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>

using namespace openocl::base;
using namespace openocl::imaging;
using namespace openocl::ui;
using namespace openocl::util;

GraphicsBase::GraphicsBase()
{
	this->brush = null;
	this->fill = null;
	this->background = new Color( Color::DIALOG );

	this->clipStack = new Stack();
	this->translateX = 0;
	this->translateY = 0;
	this->translateZ = 0;
	this->scale = 1.0;
}

GraphicsBase::~GraphicsBase()
{
	delete this->clipStack;
	delete this->brush;
	delete this->fill;
	delete this->background;
}

void
GraphicsBase::setScale( double scale )
{
	this->scale = scale;
}

void
GraphicsBase::setTranslation( int x, int y, int z )
{
	this->translateX = x;
	this->translateY = y;
	this->translateZ = z;
}

void
GraphicsBase::translate( int x, int y, int z )
{
	this->translateX += x;
	this->translateY += y;
	this->translateZ += z;
}

void
GraphicsBase::drawPoint( int x, int y )
{
	double scale = this->scale;
	x = (int) (x * scale);
	y = (int) (y * scale);
	x += this->translateX;
	y += this->translateY;

	this->rasterDrawPoint( x, y );	
}

void
GraphicsBase::drawLine( int x1, int y1, int x2, int y2 )
{
#ifdef DEBUG_OPENOCL_UI_GRAPHICSBASE
	fprintf( stderr, "GraphicsBase::drawLine: translate offset - %i : %i\n", this->translateX, this->translateY );
#endif

	double scale = this->scale;
	x1 = (int) (x1 * scale);
	x2 = (int) (x2 * scale);
	y1 = (int) (y1 * scale);
	y2 = (int) (y2 * scale);
	x1 += this->translateX;
	x2 += this->translateX;
	y1 += this->translateY;
	y2 += this->translateY;

	this->rasterDrawLine( x1, y1, x2, y2 );	
}

void
GraphicsBase::drawCurve1( int Ax, int Ay, int Bx, int By, int dev )
{
	//
	//	This function needs to use the passed parameters to determin the equation
	//	of the circle of the curve. First the point of the deviation needs to be
	//	determined
	
	double dx = Bx - Ax;
	double dy = By - Ay;

	double tan_theta = dy/dx;
	double theta = atan( tan_theta );

	int Dx = (int) (Ax + (dx/2));
	int Dy = (int) (Ay + (dy/2));
	
	int opp = (int)(dev * sin( theta ));
	int adj = (int)(dev * cos( theta ));

	int Ex = Dx;
	int Ey = Dy;

	if ( By > Ay )
	{
		Ex += opp;
	} else {
		Ex -= opp;
	}
	
	if ( Bx > Ax )
	{
		Ey -= adj;
	} else {
		Ey += adj;
	}

	this->drawLine( Ax, Ay, Bx, By );
	this->drawLine( Dx, Dy, Ex, Ey );

	//
	//	We now have three points on the circle so we can find the equation
	//	of the circle.
	
	//	d(A,C) == sqrt[ (h - Ax)^2 + (k - Ay)^2 ] == r
	//	d(B,C) == sqrt[ (h - Bx)^2 + (k - By)^2 ] == r
	//	d(E,C) == sqrt[ (h - Ex)^2 + (k - Ey)^2 ] == r
	//
	//		Equation 1
	//
	//	         sqrt[ (h - Ax)^2 + (k - Ay)^2 ] == sqrt[ (h - Bx)^2 + (k - By)^2 ]
	//	                 (h - Ax)^2 + (k - Ay)^2 == (h - Bx)^2 + (k - By)^2                           squared both sides
	//	 2h - 2.h.Ax + 2.Ax + 2k - 2.k.Ay + 2.Ay == 2h - 2.h.Bx + 2.Bx + 2k - 2.k.By + 2.By           expanded out squares
	//	          -2.h.Ax + 2.Ax - 2.k.Ay + 2.Ay == -2.h.Bx + 2.Bx - 2.k.By + 2.By                    simplified
	//	       2.h.Bx - 2.h.Ax - 2.k.Ay + 2.k.By == 2.Bx + 2.By - 2.Ax - 2.Ay                         standard form
	//	               h.Bx - h.Ax - k.Ay + k.By == Bx + By - Ax - Ay                                 divide by 2
	//	                 h(Bx - Ax) - k(Ay + By) == Bx + By - Ax - Ay                                 in terms of h and k
	//
	//
	//		Equation 2
	//
	//	        sqrt[ (h - Ax)^2 + (k - Ay)^2 ] == sqrt[ (h - Ex)^2 + (k - Ey)^2 ]
	//	                (h - Ax)^2 + (k - Ay)^2 == (h - Ex)^2 + (k - Ey)^2                            squared both sides
	//	2h - 2.h.Ax + 2.Ax + 2k - 2.k.Ay + 2.Ay == 2h - 2.h.Ex + 2.Ex + 2k - 2.k.Ey + 2.Ey            expanded out squares
	//	         -2.h.Ax + 2.Ax - 2.k.Ay + 2.Ay == -2.h.Ex + 2.Ex - 2.k.Ey + 2.Ey                     simplified
	//	      2.h.Ex - 2.h.Ax - 2.k.Ay + 2.k.Ey == 2.Ex + 2.Ey - 2.Ax - 2.Ay                          standard form
	//	              h.Ex - h.Ax - k.Ay + k.Ey == Ex + Ey - Ax - Ay                                  divide by 2
	//	                h(Ex - Ax) - k(Ay + Ey) == Ex + Ey - Ax - Ay                                  in terns of h and k
	//
	//
	//	To remove k multiply eq1 by (Ay + Ey) and Eq2 to by -(Ay + By)
	//
	//		Equation 1
	//
	//	h(Bx - Ax)(Ay + Ey) - k(Ay + By)(Ay + Ey) == (Bx + By - Ax - Ay)(Ay + Ey)                     multiply by (Ay + Ey)
	//
	//		Equation 2
	//
	//	-h(Ex - Ax)(Ay + By) + k(Ay + Ey)(Ay + By) == -(Ex + Ey - Ax - Ay)(Ay + By)                     multiply by -(Ay + By)
	//
	//	Use method of addition to solve the system (add each side to the appropriate side)
	//
	//	[h(Bx - Ax)(Ay + Ey) - k(Ay + By)(Ay + Ey)] + [-h(Ex - Ax)(Ay + By) + k(Ay + Ey)(Ay + By)] == [(Bx + By - Ax - Ay)(Ay + Ey)] + [-(Ex + Ey - Ax - Ay)(Ay + By)]
	//	                                                 h(Bx - Ax)(Ay + Ey) - h(Ex - Ax)(Ay + By) == (Bx + By - Ax - Ay)(Ay + Ey) - (Ex + Ey - Ax - Ay)(Ay + By)
	//	                                                h[(Bx - Ax)(Ay + Ey) - (Ex - Ax)(Ay + By)] == (Bx + By - Ax - Ay)(Ay + Ey) - (Ex + Ey - Ax - Ay)(Ay + By)
	//
	//	h == [(Bx + By - Ax - Ay)(Ay + Ey) - (Ex + Ey - Ax - Ay)(Ay + By)] / [(Bx - Ax)(Ay + Ey) - (Ex - Ax)(Ay + By)]
	//
	//	let a  == Bx + By - Ax - Ay
	//	let b  == Ex + Ey - Ax - Ay
	//	let ae == Ay + Ey
	//	let ab == Ay + By
	//	let ba == Bx - Ax
	//	let ea == Ex - Ax

	int a  = Bx + By - Ax - Ay;
	int b  = Ex + Ey - Ax - Ay;
	int ae = Ay + Ey;
	int ab = Ay + By;
	int ba = Bx - Ax;
	int ea = Ex - Ax;

	//	k = ((a^2 + b^2)(e-c) + (c^2 + d^2)(a-e) + (e^2+f^2)(c-a)) / (2(b(e-c)+d(a-e)+f(c-a)))
	//
	
	//
	//	Therefore
	//
	//	h == [(a * ae) - (b * ab)] / [(ba*ae) - (ea*ab)]
	
	int h = ((a*ae) - (b*ab)) / ((ba*ae) - (ea*ab));
	int h2 = ((Bx + By - Ax - Ay)*(Ay + Ey) - (Ex + Ey - Ax - Ay)*(Ay + By)) / ((Bx - Ax)*(Ay + Ey) - (Ex - Ax)*(Ay + By));
	
	//	To find k, substitute h into previous equation.
	
	//	       h(Bx - Ax) - k(Ay + By) == Bx + By - Ax - Ay                    equation 1 in terms of k and k
	//	h(Bx - Ax) - Bx - By + Ax + Ay == k(Ay + By)                           isolate k term on right-hand side
	//	                    k(Ay + By) == h(Bx - Ax) - Bx - By + Ax + Ay       swap sides
	//                          k == [h(Bx - Ax) - Bx - By + Ax + Ay] / (Ay + By)
	//
	//	Using defines from above
	//
	//	k == [(h*ba) - (-1 * a)] / (ab)

	int k = ( (h * ba) + a) / ab;
	
	//	We now use a distance formulas to find the radius r
	//	d(A,C) == sqrt[ (h - Ax)^2 + (k - Ay)^2 ] == r
		
	int r = (int) Math::squareroot( (h - Ax)*(h - Ax) + (k - Ay)*(k - Ay) );
	
	fprintf( stderr, "GraphicsBase::drawCurve: h: %3i h2: %3i k %3i r: %3i\n", h, h2, k, r );
	
	//	The equation of the circle is given by
	//	(h - x)^2 + (k - y)^2 = r^2
	
	this->drawCircle( h, k, r * 10 );
}

double
GraphicsBase::drawCurve( int Ax, int Ay, int Bx, int By, int dev )
{
	Point A( Ax, Ay );
	Point B( Bx, By );

	double angle_of_incidence = 0;
	//
	//	This function needs to use the passed parameters to determin the equation
	//	of the circle of the curve. First the point of the deviation needs to be
	//	determined
	
	//
	//	Determine points D then E.
	//
	
	double dx = abs( Bx - Ax );
	double dy = abs( By - Ay );

	double tan_theta = dy/dx;
	double theta = atan( tan_theta );

	int Dx = Ax;
	int Dy = Ay;
	int Ex = Ax;
	int Ey = Ay;
	
	if ( (Ax < Bx) && (Ay > By) )
	{
		Dx = (int)(Ax + dx/2);
		Dy = (int)(Ay - dy/2);
	
		Ex = (int)(Dx - (dev * sin( theta )));
		Ey = (int)(Dy - (dev * cos( theta )));
	}
	else if ( (Ax < Bx) && (Ay < By) )
	{
		Dx = (int)(Ax + dx/2);
		Dy = (int)(Ay + dy/2);
		
		Ex = (int)(Dx + (dev * cos( theta )));
		Ey = (int)(Dy - (dev * sin( theta )));
	}
	else if ( (Ax > Bx) && (Ay < By) )
	{
		Dx = (int)(Ax - dx/2);
		Dy = (int)(Ay + dy/2);
		
		Ex = (int)(Dx + (dev * sin( theta )));
		Ey = (int)(Dy + (dev * cos( theta )));
	}
	else if ( (Ax > Bx) && (Ay > By) )
	{
		Dx = (int)(Ax - dx/2);
		Dy = (int)(Ay - dy/2);
		
		Ex = (int)(Dx - (dev * cos( theta )));
		Ey = (int)(Dy + (dev * sin( theta )));
	}

	//this->setForeground( Color::LIGHT_GREY );
	//this->drawLine( Ax, Ay, Bx, By );
	//this->drawLine( Dx, Dy, Ex, Ey );

	//
	//	If D and E are the same point then there is no point calculating curve etc,
	//	just draw a straight line.
	//
	//	Or if the angle is ridiculously small ie the ratio of the curve length to deviation
	//	then also just draw a straight line (this gets rid of some inaccuracies).
	
	double curve_length = sqrt( dx*dx + dy*dy );
	double ratio = curve_length / (dev * 1.0);

	double arbitrary_limit = 1000.0;

	if ( (Dx == Ex) && (Dy == Ey) )
	{
#ifdef DEBUG_OPENOCL_UI_GRAPHICSBASE_DRAWCURVE
		fprintf( stderr, "GraphicsBase::drawCurve: no curve in line\n" );
#endif
		this->drawLine( Ax, Ay, Bx, By );
		angle_of_incidence = Geometry::angleOfLine( A, B );
	}
	else if ( arbitrary_limit < ratio )
	{
#ifdef DEBUG_OPENOCL_UI_GRAPHICSBASE_DRAWCURVE
		fprintf( stderr, "GraphicsBase::drawCurve: over cut off: %f: value %f\n", arbitrary_limit, ratio );
#endif
		this->drawLine( Ax, Ay, Bx, By );
		angle_of_incidence = Geometry::angleOfLine( A, B );
	}
	else
	{

	//
	//	Determine the equation of the circle that passes through points A, B and E.
	//

	double a = Ax;
	double b = Ay;
	double c = Bx;
	double d = By;
	double e = Ex;
	double f = Ey;
	
	//	
	//	k = ((a^2 + b^2)(e-c) + (c^2 + d^2)(a-e) + (e^2+f^2)(c-a)) / (2(b(e-c)+d(a-e)+f(c-a)))
	double k = ((a*a + b*b) * (e-c) + (c*c + d*d) * (a-e) + (e*e + f*f) * (c-a)) / (2 * (b * (e-c) + d * (a-e) + f * (c-a) ));

	//
	//
	//	h = ((a^2 + b^2)(f-d) + (c^2 + d^2)(b-f) + (e^2+f^2)(d-b)) / (2(a(f-d)+c(b-f)+e(d-b)))

	double h = ((a*a + b*b)*(f-d) + (c*c + d*d)*(b-f) + (e*e + f*f)*(d-b)) / (2 * (a * (f-d)+c * (b-f) + e* (d-b)));
	
	double r = Math::squareroot( (a - h)*(a - h) + (b - k)*(b - k) );

	//fprintf( stderr, "GraphicsBase::drawCurve: h: %3f k: %3f r: %3f \n", f, k, r );
	//fprintf( stderr, "GraphicsBase::drawCurve: h: %3i h2: %3i k %3i r: %3i\n", h, h2, k, r );

	//
	//	Determine the angle of the line from the center of the circle to point A.
	//	Determine the angle of the line from the center of the circle to point B.
	//

	double radians_to_degrees = 180/Math::pi();

	double angle1_adj = (double) abs( (int) (Ax - h) );
	double angle1_cos = (angle1_adj / r) ;
	double angle1_radians = acos( angle1_cos );
	double angle1_degrees = angle1_radians * radians_to_degrees;
	double angle1 = angle1_degrees;

	//fprintf( stderr, "GraphicsBase::drawCurve: a1  cos theta = %3f / %3f\n", angle1_adj, r );
	//fprintf( stderr, "GraphicsBase::drawCurve: a1  cos theta = %3f\n", angle1_cos );
	//fprintf( stderr, "GraphicsBase::drawCurve: a1  radians   = %3f\n", angle1_radians );
	//fprintf( stderr, "GraphicsBase::drawCurve: a1  degrees   = %3f\n", angle1_degrees );

	double angle2_adj = (double) abs( (int) (Bx - h) );
	double angle2_cos = angle2_adj / r;
	double angle2_radians = acos( angle2_cos );
	double angle2_degrees = angle2_radians * radians_to_degrees;
	double angle2 = angle2_degrees;

	//fprintf( stderr, "GraphicsBase::drawCurve: a2  cos theta = %3f / %3f\n", angle2_adj, r );
	//fprintf( stderr, "GraphicsBase::drawCurve: a2  cos theta = %3f\n", angle2_cos );
	//fprintf( stderr, "GraphicsBase::drawCurve: a2  radians   = %3f\n", angle2_radians );
	//fprintf( stderr, "GraphicsBase::drawCurve: a2  degrees   = %3f\n", angle2_degrees );

	//
	//	Determine which quadrant each point is in.
	//

	//fprintf( stderr, "GraphicsBase::drawCurve: before: angle1: %3i angle2: %3i\n", angle1, angle2 );

	int a1q;
	int a2q;

	if ( (Ax > h) && (Ay < k ) )
	{
		//john was here
		//fprintf( stderr, "GraphicsBase::drawCurve: angle1: 1st\n" );
		a1q = 1;	//	Top right (quadrant 1)
	}
	else if ( (Ax < h) && (Ay < k) )
	{
		//fprintf( stderr, "GraphicsBase::drawCurve: angle1: 2nd\n" );
		a1q = 2;	//	Top left (quadrant 2)
		angle1 = 180 - angle1;
	}
	else if ( (Ax < h) && (Ay > k) )
	{
		//fprintf( stderr, "GraphicsBase::drawCurve: angle1: 3rd\n" );
		a1q = 3;	//	Bottom left (quadrant 3)
		angle1 = angle1 + 180;
	}
	else
	{
		//fprintf( stderr, "GraphicsBase::drawCurve: angle1: 4th\n" );
		a1q = 4;	
		angle1 = 360 - angle1;
	} 

	if ( (Bx > h) && (By < k ) )
	{
		//fprintf( stderr, "GraphicsBase::drawCurve: angle2: 1st\n" );
		a2q = 1;	//	Top right (quadrant 1)
	}
	else if ( (Bx < h) && (By < k) )
	{
		//fprintf( stderr, "GraphicsBase::drawCurve: angle2: 2nd\n" );
		a2q = 2;	//	Top left (quadrant 2)
		angle2 = 180 - angle2;
	}
	else if ( (Bx < h) && (By > k) )
	{
		//fprintf( stderr, "GraphicsBase::drawCurve: angle2: 3rd\n" );
		a2q = 3;	//	Bottom left (quadrant 3)
		angle2 = angle2 + 180;
	}
	else
	{
		//fprintf( stderr, "GraphicsBase::drawCurve: angle2: 4th\n" );
		a2q = 4;	
		angle2 = 360 - angle2;
	} 

	//
	//	Ok, so we have the angles of point A and B, if the distance between A and B
	
	double extent_angle = fabs( angle1 - angle2 );
	if ( extent_angle > 180 )
	{
		extent_angle = 360 - extent_angle;
	}
	
	if ( angle1 < angle2 )
	{
		if ( (angle2 - angle1) > 180 )
		{
			extent_angle *= -1;
		}
	}
	else if ( angle2 < angle1 )
	{
		if ( (angle1 - angle2) < 180 )
		{
			extent_angle *= -1;
		}
	}

	//fprintf( stderr, "GraphicsBase::drawCurve: drawing from %3i to %3i (%3i)\n", angle1, angle2, extent_angle );
	
	//this->setForeground( Color::LIGHT_GREY );
	//this->drawLine( (int) h, (int) k, Ax, Ay );
	//this->drawLine( (int) h, (int) k, Bx, By );
	
	//	The equation of the circle is given by
	//	(h - x)^2 + (k - y)^2 = r^2

	int x = (int) (h - r);
	int y = (int) (k - r);
	int w = (int) (r*2);
	int a1 = (int)(angle1 * 64);
	int a2 = (int)(extent_angle * 64);

#ifdef DEBUG_OPENOCL_UI_GRAPHICSBASE
	fprintf( stderr, "GraphicsBase::drawCurve: drawArc( %i, %i, %i, %i, %i(%i), %i )\n", x, y, w, w, a1, angle1, a2 );
#endif


	//this->setForeground( Color::LIGHT_GREY );
	//this->drawCircle( (int) h, (int) k, (int) r );
	
	//this->setForeground( Color::BLACK );
	this->drawArc( x, y, w, w, a1, a2 );

	//	Angle of incidence is 90 degrees of from end arc ie (a1 + a2)
	//	Angle of incidence = a1 + a2 + pi/2
	
	angle_of_incidence = Geometry::toRadians( angle2 - 90.0 );

	}
	return angle_of_incidence;
}

void
GraphicsBase::drawCircle( int x, int y, int radius )
{
	//double rad_div_2 = radius / 2.0;
	int x1     = x - radius;
	int y1     = y - radius;
	int bredth = radius * 2;

	this->drawArc( x1, y1, bredth, bredth, 0, 360 * 64 );
}

void
GraphicsBase::drawFilledCircle( int x, int y, int radius )
{
	//double rad_div_2 = radius / 2.0;
	int x1     = x - radius;
	int y1     = y - radius;
	int bredth = radius * 2;

	this->drawFilledArc( x1, y1, bredth, bredth, 0, 360 * 64 );
}

void
GraphicsBase::drawRectangle( int x, int y, unsigned int width, unsigned int height )
{
	Region region;
	region.x1 = x;
	region.y1 = y;
	region.width = width;
	region.height = height;
	region.x2 = region.x1 + width  - 1;
	region.y2 = region.y1 + height - 1;
	
	this->drawRectangleRegion( region );
}

void
GraphicsBase::drawRectangleRegion( const Region& aRegion )
{
	Region region = aRegion;
	this->rescaleAndTranslate( region );

	this->rasterDrawRectangle( region.x1, region.y1, region.width, region.height );
}

void
GraphicsBase::drawFilledRectangle( int x, int y, unsigned int width, unsigned int height )
{
	Region region;
	region.x1 = x;
	region.y1 = y;
	region.width = width;
	region.height = height;
	region.x2 = region.x1 + width  - 1;
	region.y2 = region.y1 + height - 1;
	
	this->drawFilledRectangleRegion( region );
}

void
GraphicsBase::drawFilledRectangleRegion( const Region& aRegion )
{
	Region region = aRegion;
	
	region.scale( this->scale );
	region.translate( this->translateX, this->translateY );

	this->rasterDrawFilledRectangle( region.x1, region.y1, region.width, region.height );
}

void
GraphicsBase::drawArc( int x, int y, unsigned int width, unsigned int height, int angle1, int angle2 )
{
	int w = width;
	int h = height;
	GraphicsBase::rescaleAndTranslate( &x, &y );
	GraphicsBase::rescale( &w, &h );

	this->rasterDrawArc( x, y, w, h, angle1, angle2 );
}


void
GraphicsBase::drawFilledArc( int x, int y, unsigned int width, unsigned int height, int angle1, int angle2 )
{
	int w = width;
	int h = height;
	GraphicsBase::rescaleAndTranslate( &x, &y );
	GraphicsBase::rescale( &w, &h );

	this->rasterDrawFilledArc( x, y, w, h, angle1, angle2 );
}

void
GraphicsBase::drawPolygon( const openocl::util::Sequence& points )
{
	Sequence coords;
	
	double x, y;
	unsigned int max = points.getLength();
	for ( unsigned int i=0; i < max; i++ )
	{
		const Point& p = dynamic_cast<const Point&>( points.elementAt( i ) );
		x = p.x;
		y = p.y;
		this->rescaleAndTranslate( &x, &y );
		coords.add( new Coordinates( (int) x, (int) y ) );
	}
	this->rasterDrawPolygon( coords );
}

void
GraphicsBase::drawFilledPolygon( const openocl::util::Sequence& points )
{
	Sequence coords;
	
	double x, y;
	unsigned int max = points.getLength();
	for ( unsigned int i=0; i < max; i++ )
	{
		const Point& p = dynamic_cast<const Point&>( points.elementAt( i ) );
		x = p.x;
		y = p.y;
		this->rescaleAndTranslate( &x, &y );
		coords.add( new Coordinates( (int) x, (int) y ) );
	}
	this->rasterDrawFilledPolygon( coords );
}

void
GraphicsBase::drawString( const openocl::base::String& aString, int x, int y )
{
	GraphicsBase::rescaleAndTranslate( &x, &y );
	this->rasterDrawString( aString, x, y );
}

int
GraphicsBase::drawBoundedString( const String& aString, const Region& aRegion, flags options )
{
	int y_offset = 0;
	TextExtents* extents = this->measureTextExtents( aString );
	Region region = aRegion;
	region.height += extents->descent;
	region.x2 = region.x1 + region.width + 1;

	this->setClip( region.x1, region.y1, region.width, region.height );
	{
		GraphicsBase::rescaleAndTranslate( region );
		Region clip_region = this->getClipRegion();
		if ( clip_region.equals( region ) )
		{
			//fprintf( stderr, "GraphicsBase::drawBoundedString: %s : %i:%i\n", aString.getChars(), region.x1, region.y2 );
			int height = extents->ascent + extents->descent;

			int start = 0;
			int end = aString.getLength() - 1;
			y_offset = extents->ascent;
	
			do
			{
				start = this->drawVisibleString( aString, region, y_offset, start, end );
				y_offset += height;
			} while ( start <= end );
		}
	}
	this->popClip();
	delete extents;
	
	return y_offset;
}

void
GraphicsBase::drawImage( const OffscreenImage& anImage, int x, int y )
{
	int x1 = x;
	int y1 = y;
	GraphicsBase::rescaleAndTranslate( &x1, &y1 );
	this->rasterDrawImage( anImage, x1, y1 );
}

void
GraphicsBase::drawImage( const openocl::imaging::Image& anImage, int x, int y )
{
	GraphicsBase::rescaleAndTranslate( &x, &y );
	this->rasterDrawImage( anImage, x, y );
}

void
GraphicsBase::setClip( int x, int y, int width, int height )
{
	x = (int) (x * scale);
	y = (int) (y * scale);
	width  = (int) (width  * scale);
	height = (int) (height * scale);
	x += this->translateX;
	y += this->translateY;

	int x2 = x + width - 1;
	int y2 = y + height - 1;

	ClipArea* clip = new ClipArea();
	clip->x        = this->clipArea.x;
	clip->y        = this->clipArea.y;
	clip->width    = this->clipArea.width;
	clip->height   = this->clipArea.height;

	int clip_x2 = clip->x + clip->width - 1;
	int clip_y2 = clip->y + clip->height - 1;

	this->clipStack->push( clip );

	if ( x < this->clipArea.x )
	{
		x = this->clipArea.x;
	}
	if ( y < this->clipArea.y )
	{
		y = this->clipArea.y;
	}
	if ( clip_x2 < x2 )
	{
		x2 = clip_x2;
	}
	if ( clip_y2 < y2 )
	{
		y2 = clip_y2;
	}

	this->clipArea.x = x;
	this->clipArea.y = y;
	this->clipArea.width  = x2 - x + 1;
	this->clipArea.height = y2 - y + 1;

	this->clip( this->clipArea );
}

void
GraphicsBase::popClip()
{
	try
	{
		ClipArea* clip = (ClipArea*) this->clipStack->pop();
		this->clipArea.setTo( *clip );
		this->unclip( *clip );
		delete clip;
	} catch ( NoSuchElementException* ex ) {
		fprintf( stderr, "GraphicsBase::popClip: run out of clips!! (aborting)\n" );
		this->resetClip();
		abort();
	}
}

void
GraphicsBase::resetClip()
{
	while ( 0 < this->clipStack->getSize() )
	{
		delete this->clipStack->pop();
	}

	this->clipArea.x = 0;
	this->clipArea.y = 0;
	this->clipArea.width  = 0xFFFF;
	this->clipArea.height = 0xFFFF;

	this->unclip( this->clipArea );
}

void
GraphicsBase::drawArrowLine( int X1, int Y1, int X2, int Y2 )
{
	int length = 15;
	int jutt   = 6;

	this->rescaleAndTranslate( &X1, &Y1 );
	this->rescaleAndTranslate( &X2, &Y2 );

	//
	//		\      |
	//		b/    | dy
	//		  \  |
	//	   ________\|
	//	     dx   A
	//
	//	From the initial line specified by x1:y1 and x2:y2 the angle of the line
	//	can be found using triginometry by finding the tangent (rise/run), which
	//	will they using an arctan lookup yield the angle.
	//
	//	dx = x2 - x1
	//	dy = y2 - y1
	//
	//	Tan A = O/A
	//	Tan A = dy / dx
	//	A     = arctan( dy/dx )

	int dx = X2 - X1;
	int dy = Y2 - Y1;

	int adx = abs( dx );
	int ady = abs( dy );

	double tan_A = (ady * 1.0) / (adx * 1.0);
	double A     = atan( tan_A );

	//	The angle A is represented in radians to convert to degrees. 
	//
	//	Radians are a measure of distance of the unit cirle (circle with circumference 1)
	//	measured in fractiosn of n (pi)
	//
	//	The circumference of a complete circle is 2nr of 2n for the unit circule as r is 1.
	//	Therefore 180 degrees is n, 90 is n/2 ...
	//
	//	 
	//	180 degrees = 1 n radian
	//	1 radian = 180 degrees / n
	//	n readians = n. 180/n
	
	//double Adegrees = A * (180/Math::PI);
	//IO::err().printf( "XGraphicsContext: angle A: %f = atan( %i / %i )\n", Adegrees, dy, dx );

	//
	//	With the angle and an arbitrary distance from the point to (b) the arrowhead
	//	base, we can find the coordinate of this point using cos and sine.
	//	The Sine of an angle equals the length of the opposite side of the length of
	//	the hypotenus. The Cosine of an angle equals the length of the adjacent side
	//	over the length of the hypotenus.
	//
	//	Cos A = A/H			Sin A = O/H
	//	A = H . Cos A			O = H . Sin A

	double adj = length * cos( A );
	double opp = length * sin( A );

	//IO::err().printf( "XGraphicsContext: adj: %f opp: %f\n", adj, opp );

	//	This will give the required distance away from the point x2:y2 for our base line.
	
	int bdx = (int) adj;
	int bdy = (int) opp;

	//	The direction to move the point in each plane is determined by the 

	int sdx = ( dx ? (dx / adx ): 1 );	// either 1 or -1
	int sdy = ( dy ? (dy / ady ): 1 );

	//IO::err().printf( "XGraphicsContext: sdx: %i sdy: %i\n", sdx, sdy );
	
	int bX = X2 - (bdx * sdx);
	int bY = Y2 - (bdy * sdy);

	//IO::err().printf( "XGraphicsContext: bX(%i) = X2(%i) - (bdx(%i) * sdx(%i))\n", bX, X2, bdx, sdx );
	//IO::err().printf( "XGraphicsContext: bY(%i) = Y2(%i) - (bdy(%i) * sdy(%i))\n", bY, Y2, bdy, sdy );


	//		\  d   |
	//	     c  b/    | dy
	//		  \  |
	//	   ________\|
	//	     dx   A
	//
	
	//	We now need to determine flanking points the problem is exaclty the same as the
	//	previous one except that the angles are 90 degrees and 180 degress out. The length
	//	of the hypotinus is now half the width of the arrowhead's base (the jutt).
	//
	//	Sin C = O/H			Cos C = A/H
	//	O = H . Sin C			A = H . Cos C
	
	int    cdx = (int) (jutt * sin( A ));
	int    cdy = (int) (jutt * cos( A ));

	//IO::err().printf( "XGraphicsContext: cdx: %i cdy: %i\n", cdx, cdy );

	int cX = bX + (cdx * sdy);
	int cY = bY + (cdy * sdx * -1);
	
	int dX = bX - (cdx * sdy);
	int dY = bY - (cdy * sdx * -1);
	
	
	/*
	double dy_dx = (dy * 1.0) / (dx * 1.0);
	double inverse = (dx * 1.0) / (dy * 1.0); 

	int X3 = X2 - dy;
	int Y3 = Y2 - dx;

	int X4 = X1 + (dy - dx);

	int dx2 = (dy * dy) / dx;
	int X5 = X1 - dx2;
	*/

	//	Now actually do the drawing

	Sequence points;
	points.add( new Coordinates( X2, Y2 ) );
	points.add( new Coordinates( cX, cY ) );
	points.add( new Coordinates( dX, dY ) );

	//XPoint points[3];
	//points[0].x = X2;
	//points[0].y = Y2;
	//points[1].x = cX;
	//points[1].y = cY;
	//points[2].x = dX;
	//points[2].y = dY;
	
	this->rasterDrawLine( X1, Y1, bX, bY );
	//	XDrawLine( (Display*) this->display, this->win, (GC) this->gc, X1, Y1, bX, bY );
	this->rasterDrawFilledPolygon( points );
	//	XFillPolygon( (Display*) this->display, this->win, (GC) this->gc, points, 3, 0, CoordModeOrigin );
}

void
GraphicsBase::drawArrowHead( int x1, int y1, int x2, int y2, double size )
{
	Point B( x1, y1 );
	Point A( x2, y2 );

	double radians = openocl::imaging::Geometry::angleOfLine( B, A );
	this->drawArrowHead( x2, y2, radians, size );
}

void
GraphicsBase::drawArrowHead( int x1, int y1, double radians, double size )
{
	//	          D         // 
	//	           \        //
	//	      B     \       //
	//	             \      // 
	//	C_____________\     //
	//	               A    //

	double width = size/4;
	double pi = Math::pi();

	Point B = openocl::imaging::Geometry::pointFromOrigin( radians, size );
	Point A( x1, y1 );

	B.x = A.x - B.x;
	B.y = A.y - B.y;

	//	Add 90 deg. for C
	radians += pi/2;
	Point C = openocl::imaging::Geometry::pointFromOrigin( radians, width );
	C.x += B.x;
	C.y += B.y;

	//	Add a further 180 deg. for D
	radians += pi;
	Point D = openocl::imaging::Geometry::pointFromOrigin( radians, width );
	D.x += B.x;
	D.y += B.y;

#ifdef DEBUG_OPENOCL_UI_GRAPHICSBASE_DRAWARROWHEAD
	String As( "A" );
	String Bs( "B" );
	String Cs( "C" );
	String Ds( "D" );

	this->drawLine( B.x, B.y, A.x, A.y );
	this->drawString( As, A.x, A.y );
	this->drawString( Bs, B.x, B.y ); 
	this->drawLine( B.x, B.y, C.x, C.y );
	this->drawString( Cs, C.x, C.y ); 

	this->drawLine( B.x, B.y, D.x, D.y );
	this->drawString( Ds, D.x, D.y ); 
#endif

	Sequence points;
	points.add( A );
	points.add( C );
	points.add( D );

	this->drawFilledPolygon( points );
}

void
GraphicsBase::setBrush( const openocl::imaging::Color& color )
{
	delete this->brush;
	this->brush = new Color( color.getColor() );
}

void
GraphicsBase::setFill( const openocl::imaging::Color& color )
{
	delete this->fill;
	this->fill = new Color( color.getColor() );
}

void
GraphicsBase::setBackground( const openocl::imaging::Color& color )
{
	delete this->background;
	this->background = new Color( color.getColor() );
}

void
GraphicsBase::setFont( const Font& font ) const
{
	const_cast<GraphicsBase*>( this )->currentFont = &font;
}

//-----------------------------------------------------------------------------------
//	public constant methods
//-----------------------------------------------------------------------------------

Region
GraphicsBase::getClipRegion() const
{
	Region clip;
	clip.x1     = this->clipArea.x;
	clip.y1     = this->clipArea.y;
	clip.width  = this->clipArea.width;
	clip.height = this->clipArea.height;
	clip.x2     = clip.x1 + clip.width - 1;
	clip.y2     = clip.y1 + clip.height - 1;

	return clip;
}

bool
GraphicsBase::canDraw( Region aRegion ) const
{
	Region clipRegion = this->getClipRegion();
	this->rescaleAndTranslate( aRegion );

	return clipRegion.containsPartOf( aRegion );
}

const Color&
GraphicsBase::getBrush() const
{
	return *this->brush;
}

const Color&
GraphicsBase::getFill() const
{
	return *this->fill;
}

const Color&
GraphicsBase::getBackground() const
{
	return *this->background;
}

const Font&
GraphicsBase::getFont() const
{
	return *this->currentFont;
}

//-----------------------------------------------------------------------------------
//	private methods
//-----------------------------------------------------------------------------------

int
GraphicsBase::drawVisibleString( const String& string, const Region& box, int offset, int start, int end )
{
	int ret = 0;
	//fprintf( stderr, "GraphicsBase::drawVisibleString( %i, %i )\n", start, end );
	String* str = string.substring( start, end );
	int text_width = this->measureTextWidth( *str );
	
	if ( (box.width < text_width) && (start != end) )
	{
		int next_end = (start + (end - start)/2);
		if ( next_end == end )
		{
			next_end--;
		}
		ret = this->drawVisibleString( string, box, offset, start, next_end );
	} else {
		if ( string.getLength() - 1 == end )
		{
			this->rasterDrawString( *str, box.x1, box.y1 + offset );
			ret = end + 1;
		}
		else if ( start == end )
		{
			this->rasterDrawString( *str, box.x1, box.y1 + offset );
			ret = end + 1;
		}
		else if ( box.width < (text_width + 10) )
		{
			this->rasterDrawString( *str, box.x1, box.y1 + offset );
			ret = end + 1;
		}
		else
		{
			ret = this->drawVisibleString( string, box, offset, start, end + 1 );
		}
	}
	delete str;
	
	return ret;
}

void
GraphicsBase::rescaleAndTranslate( Region& region ) const
{
	double scale = this->scale;
	region.x1     = (int) (region.x1 * scale);
	region.y1     = (int) (region.y1 * scale);
	region.width  = (int) (region.width * scale);
	region.height = (int) (region.height * scale);
	region.x1 += this->translateX;
	region.y1 += this->translateY;
	region.x2     = region.x1 + region.width - 1;
	region.y2     = region.y1 + region.height - 1;
}

void
GraphicsBase::rescaleAndTranslate( int* x, int* y )
{
	double scale = this->scale;
	*x = (int) (*x * scale);
	*y = (int) (*y * scale);
	*x += this->translateX;
	*y += this->translateY;
}

void
GraphicsBase::rescaleAndTranslate( double* x, double* y )
{
	double scale = this->scale;
	*x = (*x * scale);
	*y = (*y * scale);
	*x += this->translateX;
	*y += this->translateY;
}

void
GraphicsBase::rescaleAndTranslate( Point& p )
{
	double scale = this->scale;
	p.x *= scale;
	p.y *= scale;
	p.x += this->translateX;
	p.y += this->translateY;
}

void
GraphicsBase::rescale( int* x, int* y )
{
	double scale = this->scale;
	*x = (int) (*x * scale);
	*y = (int) (*y * scale);
}

void
GraphicsBase::debugPrintClip() const
{
	unsigned int length = this->clipStack->getSize();
	
	for ( unsigned int i=0; i < length; i++ ) fprintf( stderr, "\t" );
	
	fprintf( stderr, "GraphicsBase::dplspse: %i:%i  --  %i:%i\n", this->clipArea.x, this->clipArea.y, this->clipArea.width, this->clipArea.height );
}
