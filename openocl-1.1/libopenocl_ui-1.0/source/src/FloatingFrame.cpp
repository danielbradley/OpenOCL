/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/FloatingFrame.h"

#include "openocl/ui/ColumnLayout.h"
#include "openocl/ui/Panel.h"
#include "openocl/ui/Screen.h"
#include "openocl/ui/Visual.h"

using namespace openocl::ui;

FloatingFrame::FloatingFrame( Visual& aVisual ) : Frame( aVisual.getDefaultScreen().createPanel( Panel::FLOATING ) )
{
	this->getTopPanel().setLayout( new ColumnLayout() );
	this->requestGeometry( 0, 0, 1280, 50, 0 );
}
