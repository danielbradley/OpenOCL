/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_DIALOG_H
#define OPENOCL_UI_DIALOG_H

#include "openocl/ui.h"
#include "openocl/ui/Frame.h"
#include "openocl/ui/Panel.h"
#include <openocl/base/event/EventListener.h>
#include <openocl/base.h>

namespace openocl {
	namespace ui {

class Dialog : public Frame
{
public:
	static bool showDialog( Panel& aPanel, Component& com, unsigned int maxWidth=800, unsigned int maxHeight=600 );

	Dialog( Panel& guardian, flags dialogType = Panel::DIALOG );
	virtual ~Dialog();

	//	public virtual methods (EventListener)
	virtual void deliverEvent( const openocl::base::event::Event& anEvent );
	
	//	public virtual methods (Frame)
	virtual void setVisible( bool aValue );

	//	public methods
	void  enterEventLoop( bool* run );
	bool* getLoopPointer();

	//	constant methods
	const openocl::base::String& getValue() const;
	bool wasConfirmed() const;


private:
	Panel& guardian;
	openocl::base::String* value;
	bool confirmed;
	bool loop;
};

};};

#endif
