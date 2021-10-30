/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_BLANK_H
#define OPENOCL_UI_BLANK_H

#include "openocl/imaging.h"
#include "openocl/ui/Component.h"

namespace openocl {
	namespace ui {

class Blank : public openocl::ui::Component
{
public:
	Blank();
	~Blank();

	virtual Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;
	virtual bool draw( GraphicsContext& gc ) const;
};

};};

#endif
