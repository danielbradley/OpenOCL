/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_MENU_H
#define OPENOCL_UI_MENU_H

#include "openocl/ui.h"
#include "openocl/ui/MenuItem.h"
#include <openocl/base/event.h>
#include <openocl/base/event/EventListener.h>

namespace openocl {
  namespace ui {

//
//	A Menu is a MenuItem that contains other MenuItems.
//	When a Menu is clicked it will display a PopupMenu
//	object that allows the user to select from the contained
//	MenuItems
//
class Menu : public openocl::ui::MenuItem, public openocl::base::event::EventListener
{
public:
	Menu( const openocl::base::String& name, const openocl::base::String& action, const openocl::base::String& label );
	~Menu();

	//	public virtual methods (Component)
	void deliverEvent( const openocl::base::event::Event& anEvent );

	//	public virtual methods (Component)
	virtual void processEvent( const openocl::base::event::Event& anEvent );
	virtual void setContainingPanel( Panel& aPanel );
	virtual void doLayout( const GraphicsContext& gc );
	
	//	public methods
	bool activateIfContains( int x, int y );
	void deactivate();
	void addMenuItem( MenuItem* aMenuItem );
	void addMenuItem( MenuItem& aMenuItem );
	void remove( MenuItem& aMenuItem )
		throw (openocl::base::NoSuchElementException*);
	void showPopup();
	void hidePopup();

	//	public virtual constant methods (Component)
	Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;
	virtual bool draw( GraphicsContext& gc ) const;

private:
	MenuGroup* menuGroup;
	PopupMenu* popup;
	bool selected;
	bool removePopup;
};

};};

#endif
