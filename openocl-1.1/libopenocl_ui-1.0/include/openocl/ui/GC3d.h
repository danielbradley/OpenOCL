/*
 *  Copyright (C) 1997-2006 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_GC3D_H
#define OPENOCL_UI_GC3D_H

#include "openocl/ui.h"
#include "openocl/ui/Component.h"
#include "openocl/ui/Region.h"
#include <openocl/base/Object.h>
#include <openocl/imaging.h>
#include <openocl/imaging/Point.h>
#include <openocl/util.h>

namespace openocl {
	namespace ui {

class GC3d : public openocl::base::Object
{
public:
	GC3d( GraphicsContext& gc, Region& aRegion );
	explicit GC3d( const GC3d& a3dGC );
	~GC3d();

	void drawPoint( openocl::imaging::Point& p );
	void drawLine( openocl::imaging::Point& start, openocl::imaging::Point& end );
	void drawPolygon( openocl::util::Sequence& ofPoints );
	void drawFilledPolygon( openocl::util::Sequence& ofPoint );

	openocl::ui::GraphicsContext& getGraphicsContext();

	double getWidth() const;
	double getHeight() const;
	double getDepth() const;

private:
	openocl::imaging::Point to2D( const openocl::imaging::Point& a3DPoint ) const;

	GraphicsContext& gc;
	Region bounds;
	double originX;
	double originY;

	double w;
	double h;
	double d;
};

};};

#endif
