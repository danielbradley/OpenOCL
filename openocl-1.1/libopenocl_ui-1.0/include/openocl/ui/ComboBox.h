/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_COMBOBOX_H
#define OPENOCL_UI_COMBOBOX_H

#include "openocl/ui/Textual.h"
#include "openocl/ui.h"
#include <openocl/base.h>
#include <openocl/base/event/EventListener.h>
#include <openocl/imaging.h>

namespace openocl {
  namespace ui {

class ComboBox : public Textual, public openocl::base::event::EventListener
{
public:
	static unsigned int tabWidth;

	ComboBox( const openocl::base::String& aString );
	~ComboBox();

	//	public virtual methods (EventListener)
	virtual void deliverEvent( const openocl::base::event::Event& anEvent );

	//	public virtual methods (Component)
	virtual void doLayout( const GraphicsContext& gc );
	virtual void processEvent( const openocl::base::event::Event& anEvent );
	virtual void setContainingPanel( Panel& aPanel );

	//	public methods
	void setDefaultEntry( const openocl::base::String& value );
	void addEntry( const openocl::base::String& value );

	//	public virtual constant methods (Component)
	virtual bool draw( GraphicsContext& gc ) const;
//	virtual Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;
//	virtual unsigned int getPreferredWidth( const GraphicsContext& gc ) const;
//	virtual unsigned int getPreferredHeight( const GraphicsContext& gc ) const;

	//	public virtual constant methods (Textual)
	virtual Dimensions getPreferredTextDimensions( const openocl::ui::GraphicsContext& gc ) const;
	virtual const openocl::base::String& getData() const;


private:
	void showPopup();
	void hidePopup();

	//Region* bounds;
	//Region* outline;
	//Region* text;
	Region* tab;

	openocl::base::String* currentSelection;

	ComboGroup* comboGroup;
	PopupMenu* popup;

	unsigned int popupWidth;
	bool active;
	bool selected;
	bool removePopup;
	
	int entries;
};

};};

#endif
