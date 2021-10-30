/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_PANELGEOMETRY_H
#define OPENOCL_UI_PANELGEOMETRY_H

#include <openocl/base/Object.h>

namespace openocl {
	namespace ui {

class PanelGeometry : public openocl::base::Object
{
public:
	PanelGeometry() : x( 0 ), y( 0 ), width( 0 ), height( 0 ), border( 0 )
	{}

	PanelGeometry& operator=( PanelGeometry pg )
	{
		this->x      = pg.x;
		this->y      = pg.y;
		this->width  = pg.width;
		this->height = pg.height;
		this->border = pg.border;
		return *this;
	}
	
	int x;
	int y;
	unsigned int width;
	unsigned int height;
	unsigned int border;
};

};};

#endif
