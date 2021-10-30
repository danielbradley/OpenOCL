/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_POPUPFRAME_H
#define OPENOCL_UI_POPUPFRAME_H

#include "openocl/ui/Frame.h"
#include "openocl/ui.h"

namespace openocl {
	namespace ui {

class PopupFrame : public Frame
{
public:
	PopupFrame( Screen& screen );
	PopupFrame( Panel& parent );
	virtual void setVisible( bool aValue );
};

};};

#endif
