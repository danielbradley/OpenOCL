/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_WIN32_WINPANEL_H
#define OPENOCL_UI_WIN32_WINPANEL_H

#include "openocl/ui/PanelGeometry.h"
#include "openocl/ui/Panel.h"
#include "openocl/ui/win32.h"
#include <openocl/base.h>
#include <openocl/base/event.h>
#include <openocl/imaging.h>

namespace openocl {
	namespace ui {
		namespace win32 {

class WinPanel : public openocl::ui::Panel
{
friend class WinScreen;
friend class WinVisual;
public:
	virtual ~WinPanel();

	//	public virtual methods (EventDispatcher)
	//virtual void dispatchEvent( openocl::base::event::Event* anEvent );

	//	public virtual methods (Component)
	virtual void processEvent( const openocl::base::event::Event& anEvent );
	virtual void setBounds( int x, int y, unsigned int width, unsigned int height );
	virtual void setName( const openocl::base::String& aName );

	//	public abstract virtual methods (Container)
	virtual void add( Component* aComponent );
	virtual void add( Component& aComponent );

	//	public abstract virtual methods (Panel)
	virtual void clear();
	virtual void requestGeometry( int x, int y, unsigned int width, unsigned int height, unsigned int border );
	virtual void requestPosition( int width, int height );
	virtual void requestSize( unsigned int width, unsigned int height );
	virtual void requestSizeLimits( openocl::ui::Dimensions minimum, openocl::ui::Dimensions maximum );
	virtual void resetCursor();
	virtual void setAlwaysOnTop( bool alwaysOnTop );
	virtual void setCursor( unsigned int shape );
	virtual void setCursor( const openocl::imaging::Image& anImage, const openocl::imaging::Image& aMask, int hotspotX, int hotspotY );
	virtual void map();
	virtual void unmap();
	virtual void raise();
	virtual void show( int state );
public:
//	virtual void refresh();
	virtual Visual& getVisual();
	virtual Screen& getScreen();

	//virtual void repaint();

	//	public virtual constant methods (EventDispatcher)
	virtual long unsigned int getEventDispatcherId() const;

	//	public virtual constant methods (Component)
	virtual Dimensions   getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;
	virtual int          getAbsoluteX() const;
	virtual int          getAbsoluteY() const;
	virtual bool         draw( GraphicsContext& aGraphics ) const;

	//	public virtual constant methods (Drawable)
	virtual unsigned long getId() const;
	virtual unsigned long getScreenId() const;
	virtual unsigned int  getDrawableWidth() const;
	virtual unsigned int  getDrawableHeight() const;
	virtual unsigned int  getDrawableDepth() const;

	//	public abstract constant methods (Panel)
	virtual bool isVisible() const;

	//	public virtual constant methods (Panel)
	virtual Panel* createChildPanel( flags type ) const;
	virtual Panel* createSubPanel() const;
	virtual void setName( const openocl::base::String& aName ) const;
	virtual const Visual& getVisual() const;
	virtual const Screen& getScreen() const;
	virtual GraphicsContext& getGraphicsContext() const;
	virtual void flush() const;
	virtual Coordinates queryOuterCoordinates() const;
	virtual PanelGeometry queryOuterGeometry() const;
	virtual PanelGeometry queryInnerGeometry() const;

protected:
	void calculateAbsoluteCoordinatesOf( openocl::base::event::PointerEvent& pe ) const;
	virtual void sendRefreshMessage();
	virtual void update();

private:
	//	private const methods (WinPanel)
	//void repositioned( const openocl::base::event::Event& anEvent );
	//void resized( const openocl::base::event::Event& anEvent );

	void doLayout( const GraphicsContext& gc );
	void redraw( GraphicsContext& gc ) const;
	void paint( GraphicsContext& gc ) const;

	WinPanel( WinScreen& aScreen, flags type, unsigned long parent );

	WinScreen& winscreen;

	flags type;
	GraphicsContext* gc;
	void* winId;
	WinPixmap* buffer;

	//PanelGeometry* geometry;
	int clientXOffset;
	int clientYOffset;
	int mapState;
	bool drawnBuffer;
	bool isMapped;


	//	If true panel will double buffer else not.
	bool doubleBuffer;
};

};};};

#endif
