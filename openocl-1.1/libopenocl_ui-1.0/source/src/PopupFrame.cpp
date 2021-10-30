/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/PopupFrame.h"

#include "openocl/ui/Panel.h"
#include "openocl/ui/Screen.h"
#include "openocl/ui/Visual.h"

using namespace openocl::ui;

PopupFrame::PopupFrame( Screen& screen ) : Frame( screen.createPanel( Panel::POPUP ) )
{
//	this->requestGeometry( 0, 0, 100, 300, 0 );
}

PopupFrame::PopupFrame( Panel& parent ) : Frame( parent.createChildPanel( Panel::POPUP ) )
{
//	this->requestGeometry( 0, 0, 100, 300, 0 );
}

//PopupFrame::PopupFrame( const Panel& parent ) : Frame( parent.createChildPanel( Panel::POPUP ) )
//{
//	this->requestGeometry( 0, 0, 100, 300, 0 );
//}

void
PopupFrame::setVisible( bool aValue )
{
	//unsigned int width = this->getPreferredWidth();
	//unsigned int height = this->getPreferredHeight();
	//this->requestSize( width, height );
	this->Frame::setVisible( aValue );
}
