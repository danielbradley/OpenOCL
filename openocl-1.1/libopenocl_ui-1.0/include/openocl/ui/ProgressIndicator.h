/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_PROGRESSINDICATOR_H
#define OPENOCL_UI_PROGRESSINDICATOR_H

#include "openocl/ui/Component.h"
#include "openocl/ui.h"
#include <openocl/base.h>

namespace openocl {
	namespace ui {

class ProgressIndicator : public openocl::ui::Component
{
public:
	static const int HORIZONTAL_BAR = 0x0001;
	static const int VERTICAL_BAR   = 0x0002;
	static const int DIGITAL        = 0x0003;

	static const int LINE           = 0x0010;
	static const int BOXES          = 0x0020;

	static const double initialProgress;

	ProgressIndicator( flags TYPE );
	~ProgressIndicator();

	virtual void processEvent( const openocl::base::event::Event& anEvent );
	virtual void doLayout( const GraphicsContext& gc );

	void setProgress( double progress );

	virtual Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;
	virtual bool draw( GraphicsContext& gc ) const;

private:
	flags typeAndStyle;
	Region* progressBar;
	double progress;
	unsigned int progressLength;
	openocl::base::String* progressString;

	unsigned int thickness;
};

};};

#endif
