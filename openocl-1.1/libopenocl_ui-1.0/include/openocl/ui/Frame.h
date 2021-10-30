/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_FRAME_H
#define OPENOCL_UI_FRAME_H

#include "openocl/ui.h"

#include <openocl/base/Object.h>
#include <openocl/base/Exception.h>
#include <openocl/base/event.h>
#include <openocl/base/event/EventListener.h>

namespace openocl {
	namespace ui {

class Frame : public openocl::base::Object, public openocl::base::event::EventListener
{
public:
	virtual ~Frame();

	//	public virtual methods (EventListener)
	void deliverEvent( const openocl::base::event::Event& anEvent );

	void requestGeometry( int x, int y, unsigned int width, unsigned int height, unsigned int border );
	void requestPosition( int x, int y );
	void requestSize( unsigned int width, unsigned int height );
	void positionAndResize( int x, int y );
	void positionAndResize( int x, int y, unsigned int maxWidth, unsigned int maxHeight );
	void resize( unsigned int maxWidth, unsigned int maxHeight );
	void setAlwaysOnTop( bool alwaysOnTop );
	void setName( const openocl::base::String& aName );
	void setVisible( bool aValue );
	void show( int state );
	void toFront();

	//void add( openocl::mvc::View& aView );
	void add( Component& aViewComponent );
	void remove( Component& aViewComponent );


	Panel& getTopPanel() const;
	//openocl::mvc::Controller& getController() const;
	
	unsigned int getPreferredWidth();
	unsigned int getPreferredHeight();
	bool isVisible() const;

protected:
	Frame( Panel* aPanel );

private:
	Panel* panel;
	//bool visible;
};

};};

#endif
