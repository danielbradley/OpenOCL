/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_PANEL_H
#define OPENOCL_UI_PANEL_H

#include "openocl/ui/Container.h"
#include "openocl/ui/Coordinates.h"
#include "openocl/ui/Drawable.h"
#include "openocl/ui/PanelGeometry.h"
#include "openocl/ui.h"
#include <openocl/base/event/EventDispatcher.h>
#include <openocl/base/event.h>
#include <openocl/imaging.h>
#include <openocl/threads.h>

namespace openocl {
  namespace ui {

// Interface
class Panel : public openocl::ui::Container,
              public openocl::ui::Drawable,
              public openocl::base::event::EventDispatcher
{
public:
	static const mask PANEL_MASK        = 0x00F1;
	static const mask MANAGED_MASK      = 0xFF11;
	static const mask UNMANAGED_MASK    = 0xFF21;

	static const int PANEL                   = 0x0001;
	static const int	MANAGED              = 0x0011;
	static const int		APPLICATION      = 0x0111;
	static const int		DIALOG           = 0x0211;
	static const int			FIXED_DIALOG = 0x1211;
	static const int		DOCUMENT         = 0x0411;
	static const int		FLOATING         = 0x0811;
	static const int	UNMANAGED            = 0x0021;
	static const int		POPUP            = 0x0121;
	static const int		CHILD            = 0x0221;

	static const int	SHOW         = 1;
	static const int	RESTORED     = 2;
	static const int	MINIMIZED    = 3;
	static const int	MAXIMIZED    = 4;
	static const int	HIDDEN       = 5;
	static const int	ACTIVE       = 6;
	static const int	INACTIVE     = 7;

	Panel();
	virtual ~Panel();

	PanelGeometry& getOuterGeometry();
	PanelGeometry& getInnerGeometry();

	//	public abstract virtual methods (EventDispatcher)
	virtual void dispatchEvent( openocl::base::event::Event* anEvent );

	//	public abstract virtual methods (Component)
	virtual void processEvent( const openocl::base::event::Event& anEvent ) = 0;
	virtual void setBounds( int x, int y, unsigned int width, unsigned int height ) = 0;
	virtual void setName( const openocl::base::String& aName ) = 0;

	//	public abstract virtual methods (Container)
	virtual void add( Component* aComponent ) = 0;
	virtual void add( Component& aComponent ) = 0;

	//	public abstract virtual methods (Panel)
	virtual void clear() = 0;
	virtual void requestGeometry( int x, int y, unsigned int width, unsigned int height, unsigned int border ) = 0;
	virtual void requestPosition( int x, int y ) = 0;
	virtual void requestSize( unsigned int width, unsigned int height ) = 0;

	virtual void resetCursor() = 0;
	virtual void setAlwaysOnTop( bool alwaysOnTop ) = 0;
	virtual void setCursor( unsigned int shape ) = 0;
	virtual void setCursor( const openocl::imaging::Image& anImage, const openocl::imaging::Image& aMask, int hotspotX, int hotspotY ) = 0;
	virtual void map() = 0;
	virtual void unmap() = 0;
	virtual void raise() = 0;
	virtual void show( int state ) = 0;

public:
	virtual Visual& getVisual() = 0;
	virtual Screen& getScreen() = 0;

	//	public methods
	bool repositioned( const openocl::base::event::RepositionEvent& aRepositionEvent );
	bool resized( const openocl::base::event::ResizeEvent& aResizeEvent );

	void requestRefresh();
	void redrawAll();

	void setCloseBoolean( bool* loop );
	void setContinueClose( bool continueClose );
	
	//	public pseudo-const methods
	//void refresh() const;		// called by other threads to get panel to update: tenuously multithreaded.
	void relayout() const;
	void redraw() const;
	void repaint() const;

	//	public virtual constant methods (EventDispatcher)
	virtual long unsigned int getEventDispatcherId() const = 0;

	//	public virtual constant methods (Component)
	virtual Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const = 0;
	virtual int        getAbsoluteX() const = 0;
	virtual int        getAbsoluteY() const = 0;
	virtual bool       draw( GraphicsContext& aGraphics ) const = 0;

	//	public virtual constant methods (Drawable)
	virtual unsigned long getId() const = 0;
	virtual unsigned long getScreenId() const = 0;
	virtual unsigned int  getDrawableWidth() const = 0;
	virtual unsigned int  getDrawableHeight() const = 0;
	virtual unsigned int  getDrawableDepth() const = 0;	

	//	public abstract constant methods (Panel)
	virtual bool isVisible() const = 0;

	//	public virtual constant methods
	virtual Panel*           createChildPanel( flags type ) const = 0;
	virtual Panel*           createSubPanel() const = 0;
	virtual void             setName( const openocl::base::String& aName ) const = 0;
	virtual const Visual&    getVisual() const = 0;
	virtual const Screen&    getScreen() const = 0;
	virtual GraphicsContext& getGraphicsContext() const = 0;
	virtual void             flush() const = 0;
	virtual Coordinates      queryOuterCoordinates() const = 0;
	virtual PanelGeometry         queryOuterGeometry() const = 0;
	virtual PanelGeometry         queryInnerGeometry() const = 0;

	//	public const methods
	const PanelGeometry& getOuterGeometry() const;
	const PanelGeometry& getInnerGeometry() const;
	Coordinates     getOuterCoordinates() const;
	Coordinates     getOffsetCoordinates() const;

//  virtual const AbstractDisplay& getAbstractDisplay() const = 0;
//  virtual unsigned int getScreen() const = 0;

protected:
	//	protected abstract virtual methods
	virtual void doLayout( const GraphicsContext& gc ) = 0;
	virtual void sendRefreshMessage() = 0;
	virtual void update() = 0;

	//	protected methods (Panel)
	void beginClose();
	void finishClose();

	//	protected abstract virtual constant methods
	virtual void redraw( GraphicsContext& gc ) const = 0;
	virtual void paint( GraphicsContext& gc ) const = 0;

	virtual void calculateAbsoluteCoordinatesOf( openocl::base::event::PointerEvent& aPointerEvent ) const = 0;

	//	protected constant methods
	void update() const;

	static const int REFRESH   = 0x1111;
	static const int RELAYOUT  = 0x0111;
	static const int REDRAW    = 0x0011;
	static const int REPAINT   = 0x0001;
		  
	int status;

private:
	void checkRefresh();

	PanelGeometry* outer;
	PanelGeometry* inner;

	openocl::threads::Mutex* refreshCountMutex;
	int refreshCount;
	
	bool isWithinDispatchEvent;
	bool continueClose;
	bool* closeBoolean;
};

};};

#endif
