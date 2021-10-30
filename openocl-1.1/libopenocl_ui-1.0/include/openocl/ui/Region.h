/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_REGION_H
#define OPENOCL_UI_REGION_H

#include <openocl/base/Object.h>
#include <openocl/base.h>

namespace openocl {
	namespace ui {

struct _SRegion
{
	int x1;
	int y1;
	int x2;
	int y2;
	int width;
	int height;
};

typedef _SRegion SRegion;

class Region : public openocl::base::Object
{
public:
	Region();
	Region( const Region& region );
	~Region();

	int x1;
	int y1;
	int x2;
	int y2;
	int width;
	int height;


	Region& operator=( Region aR );

	void scale( double ratio );
	void translate( int x, int y );

	Region shrunkBy( unsigned int value ) const;

	bool contains( int x, int y ) const;
	bool contains( Region aRegion ) const;
	bool containsPartOf( Region aRegion ) const;
	bool equals( Region aRegion ) const;
	void print( void* stream ) const;
	bool isValid() const;
};

};};

#endif
