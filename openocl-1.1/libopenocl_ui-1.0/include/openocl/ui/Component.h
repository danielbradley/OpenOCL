/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_COMPONENT_H
#define OPENOCL_UI_COMPONENT_H

#include "openocl/ui.h"
#include "openocl/ui/Coordinates.h"
#include "openocl/ui/Dimensions.h"
#include "openocl/ui/Region.h"
#include "openocl/ui/UIException.h"
#include "openocl/util.h"

#include <openocl/base.h>
#include <openocl/base/Object.h>
#include <openocl/base/event/EventHandler.h>
#include <openocl/base/NoSuchElementException.h>
#include <openocl/base/event.h>
#include <openocl/imaging.h>

namespace openocl {
  namespace ui {

class Bounds
{
public:
	int x;
	int y;
	unsigned int width;
	unsigned int height;
};

//Abstract
class Component : public openocl::base::Object,
                  public openocl::base::event::EventHandler
{
public:
	static unsigned int eventsFired;
	static unsigned int eventsForwarded;

	Component();
	Component( const openocl::base::String& name );
	virtual ~Component();

	//	abstract public virtual constant methods
	virtual Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width = 0xFFFF, unsigned int height = 0xFFFF ) const = 0;
	virtual bool       drawComponent( GraphicsContext& gc ) const;
	virtual bool       draw( GraphicsContext& gc ) const;

	//	EventHandler methods
	virtual void addEventListener( openocl::base::event::EventListener& anEventListener );
	virtual void fireEvent( const openocl::base::event::Event& anEvent );
	virtual void removeEventListener( openocl::base::event::EventListener& anEventListener );

	//	public virtual methods
	virtual void setBounds( int anX, int aY, unsigned int aWidth, unsigned int aHeight );
	virtual void resetContainingPanel();
	virtual void setContainingPanel( Panel& aPanel );
	virtual void doLayout( const GraphicsContext& gc );
	virtual void forwardEvent( const openocl::base::event::Event& anEvent );
	virtual void processEvent( const openocl::base::event::Event& anEvent );

	//	public methods (Component)
	void   fireRefreshEvent( const openocl::base::event::Event& anEvent );
	void   forwardChangeEvent();
	void   forwardRedrawAllChangeEvent();
	void   fireChangeEvent( unsigned int value = 0 );
	void   setBoundsChanged( bool haveChanged );
	void   setChanged( bool changed ) const;
	void   setEditMode( bool editMode );
	void   setEventMask( flags anEventMask );
	void   setComponentLayoutHints( flags layoutHints );
	void   setName( const openocl::base::String& name );
	void   setBoundsX( int x );
	void   setBoundsY( int y );
	void   setEdge( unsigned int size );
	void   setMargin( unsigned int size );
	void   setBorder( unsigned int size );
	void   setPadding( unsigned int size );
	void   setLineHeight( unsigned int size );

	//	public virtual constant methods
	virtual Coordinates  getAbsoluteCoordinates() const;
	virtual	int          getAbsoluteX() const;
	virtual	int          getAbsoluteY() const;

	virtual Region       getBounds() const;
	virtual Region       getEdgeRegion() const;
	virtual Region       getMarginRegion() const;
	virtual Region       getBorderRegion() const;
	virtual Region       getContentRegion() const;
	virtual Region       getBox() const;	// deprecated

	virtual unsigned int getEdge() const;
	virtual unsigned int getMargin() const;
	virtual unsigned int getBorder() const;
	virtual unsigned int getPadding() const;
	virtual unsigned int getLineHeight() const;

	virtual unsigned int getDrawableWidth() const;
	virtual unsigned int getDrawableHeight() const;

	//	public constant methods
	const Panel&                 getContainingPanel() const throw (openocl::base::NoSuchElementException*);
	Panel&                       getContainingPanel() throw (openocl::base::NoSuchElementException*);
	const openocl::base::String& getName() const;
//	unsigned int                 getPadding() const;
	unsigned int                 getContentInset() const;

	int getX() const;
	int getY() const;

	unsigned int getWidth() const;
	unsigned int getHeight() const;

	/**
	 *  For subclasses that have borders, these methods may be 
	 *  overridden to return the actual amount of space available for 
	 *  drawing by the component.
	 */
	bool contains( int anX, int aY ) const;
	bool haveBoundsChanged() const;
	bool hasChanged() const;
	bool hasContainingPanel() const;
	bool isEditMode() const;
protected:
	//	protected methods
	void drawInward3DBox( GraphicsContext& gc, const Region& aRegion, const openocl::imaging::Color& light, const openocl::imaging::Color& shade ) const;
	void drawOutward3DBox( GraphicsContext& gc, const Region& aRegion, const openocl::imaging::Color& light, const openocl::imaging::Color& shade ) const;

	bool hasFocus() const;

	flags eventMask;

private:
	Panel* containingPanel;
	openocl::base::String* name;

  /**
   *  This is set by Component::setBounds(...)
   *
   *  This describes the bounds of a compoent, x:y says where the origin
   *  of this component is in terms of its ancestor Panel. Width, and
   *  height give the width and height of the component.
   */                 
	Bounds       bounds;
	Region       edgeRegion;
	Region       marginBounds;
	
	unsigned int edge;		//	getEdgeRegion
	unsigned int margin;		//	getMarginRegion
	unsigned int border;		//	getBorderRegion
	unsigned int padding;		//	getContentRegion

	unsigned int lineHeight;

	flags        hints;
	bool         boundsChanged;
	bool         changed;
	bool         focus;
	bool         editMode;
	
	openocl::util::Sequence* listeners;

	Dimensions lastPreferredDimensions;
};

};};

#endif
