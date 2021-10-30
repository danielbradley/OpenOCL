/*
 *  Copyright (C) 1997-2006 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/GC3d.h"
#include "openocl/ui/GraphicsContext.h"
#include <openocl/base/Math.h>
#include <openocl/imaging/Point.h>
#include <openocl/util/Sequence.h>

using namespace openocl::base;
using namespace openocl::imaging;
using namespace openocl::ui;
using namespace openocl::util;

GC3d::GC3d( GraphicsContext& aGraphicsContext, Region& aRegion )
: Object(), gc( aGraphicsContext ), bounds( aRegion )
{
	this->originX = aRegion.x1;
	this->originY = aRegion.y2;
	this->w = aRegion.width;
	this->h = aRegion.height;
	this->d = Math::max( aRegion.width, aRegion.height );
}

GC3d::GC3d( const GC3d& a3dGC ) : Object(), gc( a3dGC.gc )
{
	this->originX = a3dGC.originX;
	this->originY = a3dGC.originY;
	this->w = a3dGC.w;
	this->h = a3dGC.h;
	this->d = a3dGC.d;
}

GC3d::~GC3d()
{}

void
GC3d::drawPoint( Point& p )
{
	Point P = to2D( p );

	this->gc.setClip( this->bounds.x1, this->bounds.y1, this->bounds.width, this->bounds.height );
	this->gc.drawPoint( (int) P.x, (int) P.y );
	this->gc.popClip();
}

void
GC3d::drawLine( Point& start, Point& end )
{
	Point Start = to2D( start );
	Point End   = to2D( end );

	this->gc.setClip( this->bounds.x1, this->bounds.y1, this->bounds.width, this->bounds.height );
	this->gc.drawLine( (int) Start.x, (int) Start.y, (int) End.x, (int) End.y );
	this->gc.popClip();
}

void
GC3d::drawPolygon( Sequence& ofPoints )
{
	unsigned int max = ofPoints.getLength();
	
	Sequence points;
	
	for ( unsigned int i=0; i < max; i++ )
	{
		const Point& point = dynamic_cast<const Point&>( ofPoints.elementAt( i ) );
		points.add( new Point( this->to2D( point ) ) );
	}
	
	this->gc.setClip( this->bounds.x1, this->bounds.y1, this->bounds.width, this->bounds.height );
	this->gc.drawPolygon( points );
	this->gc.popClip();
}

void
GC3d::drawFilledPolygon( Sequence& ofPoints )
{
	unsigned int max = ofPoints.getLength();
	
	Sequence points;
	
	for ( unsigned int i=0; i < max; i++ )
	{
		const Point& point = dynamic_cast<const Point&>( ofPoints.elementAt( i ) );
		points.add( new Point( this->to2D( point ) ) );
	}
	
	this->gc.setClip( this->bounds.x1, this->bounds.y1, this->bounds.width, this->bounds.height );
	this->gc.drawFilledPolygon( points );
	this->gc.popClip();
}

GraphicsContext&
GC3d::getGraphicsContext()
{
	return this->gc;
}

double
GC3d::getWidth() const
{
	return this->w;
}

double
GC3d::getHeight() const
{
	return this->h;
}

double
GC3d::getDepth() const
{
	return this->d;
}

Point
GC3d::to2D( const Point& a3DPoint ) const
{
	Point twoD;
	
	double hy = a3DPoint.y/2;
	twoD.x = this->originX + (a3DPoint.x + hy);
	twoD.y = this->originY - (a3DPoint.z + hy);
	return twoD;
}

