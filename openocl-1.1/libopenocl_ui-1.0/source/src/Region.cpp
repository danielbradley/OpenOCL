/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui.h"
#include "openocl/ui/Region.h"

#include <cstdio>

using namespace openocl::ui;

Region::Region()
: Object(), x1( 0 ), y1( 0 ), x2( 0 ), y2( 0 ), width( 1 ), height( 1 )
{}

Region::Region( const Region& region ) :
Object(),
x1( region.x1 ),
y1( region.y1 ),
x2( region.x2 ),
y2( region.y2 ),
width( region.width ),
height( region.height )
{}

Region::~Region()
{}

Region&
Region::operator=( Region aR )
{
	this->x1     = aR.x1;
	this->y1     = aR.y1;
	this->width  = aR.width;
	this->height = aR.height;
	this->x2     = aR.x2;
	this->y2     = aR.y2;
	return *this;
}

void
Region::scale( double ratio )
{
	this->x1     = (int) (this->x1 * ratio);
	this->y1     = (int) (this->y1 * ratio);
	this->width  = (int) (this->width * ratio);
	this->height = (int) (this->height * ratio);
	this->x2     = this->x1 + this->width;
	this->y2     = this->y1 + this->height;
}

void
Region::translate( int x, int y )
{
	this->x1 += x;
	this->x2 += x;
	this->y1 += y;
	this->y2 += y;
}

bool
Region::contains( int x, int y ) const
{
	bool contained = true;
	contained &= ( (this->x1 <= x) && (x <= this->x2) );
	contained &= ( (this->y1 <= y) && (y <= this->y2) );
	return contained;
}

bool
Region::contains( Region aRegion ) const
{
	return ( (this->x1 <= aRegion.x1) && (this->y1 <= aRegion.y1) && (aRegion.x2 <= this->x2) && (aRegion.y2 <= this->y2 ) );
}

bool
Region::containsPartOf( Region com ) const
{
	bool contained_x = false;
	bool contained_y = false;
	
	if ( (this->x1 <= com.x1) && (com.x1 <= this->x2) )
	{
		contained_x = true;
	}
	else if ( (this->x1 <= com.x2) && (com.x2 <= this->x2 ) )
	{
		contained_x = true;
	}
	else if ( (com.x1 < this->x1) && (this->x2 < com.x2 ) )
	{
		contained_x = true;
	}

	if ( (this->y1 <= com.y1) && (com.y1 <= this->y2) )
	{
		contained_y = true;
	}
	else if ( (this->y1 <= com.y2) && (com.y2 <= this->y2 ) )
	{
		contained_y = true;
	}
	else if ( (com.y1 < this->y1) && (this->y2 < com.y2 ) )
	{
		contained_y = true;
	}

	return (contained_x && contained_y);
}

bool
Region::equals( Region aRegion ) const
{
	bool equals = true;
	
	if ( this->width != aRegion.width )
	{
		equals = false;
	}
	else if ( this->height != aRegion.height )
	{
		equals = false;
	}
	else if ( this->x1 != aRegion.x1 )
	{
		equals = false;
	}
	else if ( this->y1 != aRegion.y1 )
	{
		equals = false;
	}
	else if ( this->x2 != aRegion.x2 )
	{
		equals = false;
	}
	else if ( this->y2 != aRegion.y2 )
	{
		equals = false;
	}
	return equals;
}

void
Region::print( void* stream ) const
{
	fprintf( (FILE*) stream, "Region: x1: %3i <-- %3i --> x2: %3i\ty1: %3i <-- %3i --> y2: %3i\n",
		this->x1, this->width, this->x2, this->y1, this->height, this->y2 );
}

bool
Region::isValid() const
{
	bool valid = (this->width == (this->x2 - this->x1 + 1));
	valid &= (this->height == (this->y2 - this->y1 + 1));
	valid &= (0 < this->width);
	valid &= (0 < this->height);
	
	return valid;
}

Region
Region::shrunkBy( unsigned int value ) const
{
	Region shrunk = *this;

	shrunk.x1 += value;
	shrunk.x2 -= value;
	shrunk.y1 += value;
	shrunk.y2 -= value;
	shrunk.width  = shrunk.x2 - shrunk.x1 + 1;
	shrunk.height = shrunk.y2 - shrunk.y1 + 1;

	return shrunk;
}
