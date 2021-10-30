/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_PANE_H
#define OPENOCL_UI_PANE_H

#include "openocl/ui/Container.h"
#include "openocl/ui/LayoutManager.h"
#include <openocl/util.h>

namespace openocl {
	namespace ui {

class Pane : public openocl::ui::Container
{
public:
	Pane();
	Pane( unsigned int width, unsigned int height );
	virtual ~Pane();

	void setPaneWidth( unsigned int width );
	void setPaneHeight( unsigned int height );

	virtual Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;
//	unsigned int getPreferredWidth( const GraphicsContext& gc ) const;
//	unsigned int getPreferredHeight( const GraphicsContext& gc ) const;
private:
	unsigned int paneWidth;
	unsigned int paneHeight;
};

};};

#endif
