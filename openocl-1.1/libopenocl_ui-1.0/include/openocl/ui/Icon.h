/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_ICON_H
#define OPENOCL_UI_ICON_H

#include "openocl/ui/Selector.h"
#include "openocl/ui.h"
#include <openocl/base/event/EventListener.h>
#include <openocl/imaging.h>

namespace openocl {
  namespace ui {

class Icon : public Selector, public openocl::base::event::EventListener
{
public:
	Icon( const openocl::base::String& name );
	Icon( const openocl::base::String& name, const openocl::base::String& command );
	Icon( const openocl::base::String& name, const openocl::base::String& command, const openocl::base::String& data );
	~Icon();

	//	public virtual methods (EventListener)
	virtual void deliverEvent( const openocl::base::event::Event& anEvent );

	//	public virtual methods (Component)
	virtual void processEvent( const openocl::base::event::Event& anEvent );
	virtual void doLayout( const openocl::ui::GraphicsContext& gc );

	//	public methods
	void setImage( const openocl::ui::OffscreenImage* anImage );
	void setImage( const openocl::ui::OffscreenImage& anImage );
	void setImageId( const openocl::base::String& anImageId );
	void setLabel( const openocl::base::String& aLabel );
	void setMenu( PopupMenu& aPopupMenu );
	void setMaxPreferredWidth( unsigned int width );
	void setMinPreferredWidth( unsigned int width );

	//	public virtual constant methods (Component)
	virtual bool draw( GraphicsContext& gc ) const;
	virtual Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;

	//	public constant methods
	int getPreferredX() const;
	int getPreferredY() const;

private:
	void init();
	void obtainIconImage( const GraphicsContext& gc ) const;

	Region* picture;
	Region* text;

	unsigned int minPreferredWidth;
	unsigned int maxPreferredWidth;

	openocl::base::String* imageId;
	const OffscreenImage* image;

	IconText* iconText;

	bool selected;
	bool hoverOver;
	bool pressed;
	
	static unsigned int size;
};

};};

#endif
