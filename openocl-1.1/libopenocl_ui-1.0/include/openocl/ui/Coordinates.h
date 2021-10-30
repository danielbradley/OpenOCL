/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_COORDINATES_H
#define OPENOCL_UI_COORDINATES_H

#include <openocl/base.h>
#include <openocl/base/Object.h>
#include <openocl/imaging/Point.h>

namespace openocl {
	namespace ui {

class Coordinates : public openocl::base::Object
{
public:
	Coordinates() : Object(), x( 0 ), y( 0 )
	{}
	Coordinates( int x, int y ) : Object(), x( x ), y( y )
	{}
	Coordinates( const openocl::imaging::Point& point )
	: Object(), x( (int) point.x ), y( (int) point.y )
	{}
	Coordinates( const Coordinates& coord )
	: Object(), x( coord.x ), y( coord.y )
	{}
	~Coordinates()
	{}

	Coordinates& operator=( Coordinates c )
	{
		this->x = c.x;
		this->y = c.y;
		return *this;
	}

	int x;
	int y;
};

};};

#endif
