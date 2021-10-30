/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_CARBON_CARBONPANEL_H
#define OPENOCL_UI_CARBON_CARBONPANEL_H

#include "openocl/ui/Panel.h"
#include "openocl/ui/carbon.h"
#include "openocl/ui.h"
#include <openocl/base.h>
#include <openocl/base/event.h>
#include <openocl/imaging.h>


namespace openocl {
	namespace ui {
		namespace carbon {

class CarbonPanel : public openocl::ui::Panel
{
friend class CarbonGraphicsContext;
friend class CarbonVisual;
public:
	virtual ~CarbonPanel();

	//	public virtual methods (Component)
	virtual void add( Component* aComponent );
	virtual void add( Component& aComponent );
	virtual void processEvent( const openocl::base::event::Event& anEvent );
	virtual void setBounds( int x, int y, unsigned int width, unsigned int height );
	virtual void setName( const openocl::base::String& aName );

	//	public virtual methods (EventDispatcher)
	virtual void dispatchEvent( openocl::base::event::Event* anEvent );

	//	public virtual methods (Panel)
	virtual void clear();
	virtual void requestGeometry( int x, int y, unsigned int width, unsigned int height, unsigned int border );
	virtual void requestPosition( int width, int height );
	virtual void requestSize( unsigned int width, unsigned int height );
	virtual void resetCursor();
	virtual void setCursor( unsigned int shape );
	virtual void setCursor( const openocl::imaging::Image& anImage, const openocl::imaging::Image& aMask, int hotspotX, int hotspotY );
	virtual void map();
	virtual void unmap();
	virtual void show( int type );

	virtual void update();
	virtual void flush() const;

	//	public virtual constant methods (Component)
	virtual int getAbsoluteX() const;
	virtual int getAbsoluteY() const;
	virtual bool draw( GraphicsContext& aGraphics ) const;

	//	public virtual constant methods (Drawable)
	virtual unsigned int getDrawableWidth() const;
	virtual unsigned int getDrawableHeight() const;
	virtual unsigned int getDrawableDepth() const;
	virtual unsigned long int getId() const;
	virtual unsigned int getScreen() const;

	//	public virtual constant methods (EventDispatcher)
	virtual long unsigned int getEventDispatcherId() const;


	//	public virtual constant methods (Panel)
	virtual void setName( const openocl::base::String& aName ) const;
	virtual const Visual& getVisual() const;
	virtual GraphicsContext& getGraphicsContext() const;
	virtual Panel* createChildPanel( flags type ) const;
	virtual Panel* createSubPanel() const;

//  virtual const AbstractDisplay& getAbstractDisplay() const = 0;
//  virtual unsigned int getScreen() const = 0;

	bool redraw( openocl::ui::GraphicsContext& gc );
	void setBoundsAndGeometry( int x, int y, unsigned int width, unsigned int height );

private:
	CarbonPanel( const CarbonVisual& aVisual, flags type );

	const CarbonVisual* visual;
	void* winId;
	void* contentId;
	GraphicsContext* gc;
	
	Geometry* geometry;
};

};};};

#endif
