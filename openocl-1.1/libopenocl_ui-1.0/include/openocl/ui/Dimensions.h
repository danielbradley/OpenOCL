/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_DIMENSIONS_H
#define OPENOCL_UI_DIMENSIONS_H

#include <openocl/base.h>
#include <openocl/base/Object.h>

namespace openocl {
	namespace ui {

class Dimensions : public openocl::base::Object
{
public:
	Dimensions() : Object(), width( 0 ), height( 0 )
	{}
	Dimensions( unsigned int width, unsigned int height ) : width( width ), height( height )
	{}
	Dimensions( const Dimensions& dim ) : Object(), width( dim.width ), height( dim.height )
	{}
	~Dimensions()
	{}

	//
	//	Usage:
	//
	//	Dimensions d;
	//
	//	d = component.getPreferredDimensions( width, height );
	
	Dimensions& operator=( Dimensions aD )
	{
		this->width  = aD.width;
		this->height = aD.height;
		return *this;
	}

	unsigned int width;
	unsigned int height;
};

};};

#endif
