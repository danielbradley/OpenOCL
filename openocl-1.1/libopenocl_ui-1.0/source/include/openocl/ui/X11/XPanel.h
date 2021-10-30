/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_X11_XPANEL_H
#define OPENOCL_UI_X11_XPANEL_H

//#include "openocl/ui/Container.h"
#include "openocl/ui/Panel.h"
#include "openocl/ui/Drawable.h"

#include <openocl/base.h>
#include <openocl/base/event/EventDispatcher.h>
#include <openocl/base/event.h>
#include "openocl/ui.h"
#include "openocl/ui/X11.h"

namespace openocl {
  namespace ui {
    namespace X11 {

/*
class Geometry
{
public:
	Geometry()
	{
		this->x = 0;
		this->y = 0;
		this->height = 0;
		this->width = 0;
		this->borderWidth = 0;
	}
  unsigned int x;
  unsigned int y;
  unsigned int height;
  unsigned int width;
  unsigned int borderWidth;
};
*/

class XPanel
: public openocl::ui::Panel
{
friend class XScreen;
friend class XVisual;
friend class XGraphicsContext;

public:
	virtual ~XPanel();

	//	public virtual methods (EventDispatcher)
	//virtual void dispatchEvent( openocl::base::event::Event* anEvent );

	//	public virtual methods (Component)
	virtual void processEvent( const openocl::base::event::Event& anEvent );
	virtual void setBounds( int x, int y, unsigned int width, unsigned int height );
	virtual void setName( const openocl::base::String& aName );

	//	public virtual methods (Container)
	virtual void add( Component* aComponent ); 
	virtual void add( Component& aComponent ); 

	//	public virtual methods (Panel)
	/**
	 *  This method creates a subpanel and returns its pointer.
	 *  Note that the returned panel must be added to the panel
	 *  that it was created from. The affect of adding it to other
	 *  things is unknown, but probably not good.
	 */
	virtual void clear();
	virtual void requestGeometry( int x, int y, unsigned int width, unsigned int height, unsigned int border );
	virtual void requestPosition( int x, int y );
	virtual void requestSize( unsigned int width, unsigned int height );
	virtual void resetCursor();
	virtual void setAlwaysOnTop( bool alwaysOnTop );
	virtual void setCursor( unsigned int shape );
	virtual void setCursor( const openocl::imaging::Image& anImage, const openocl::imaging::Image& aMask, int hotspotX, int hotspotY );
	virtual void map();
	virtual void unmap();
	virtual void raise();
	virtual void show( int state );
	virtual void sendRefreshMessage();
	virtual void update();
	virtual void refresh();

	//	public methods
	//void setGeometryAndBounds( const openocl::base::event::Event& configureEvent );
	void setSize( unsigned int width, unsigned int height );
	void redraw( GraphicsContext& gc ) const;
	void doLayout( const GraphicsContext& gc );
	void reallyDoLayout( const GraphicsContext& gc );

	//	public virtual constant methods (EventDispatcher)
	virtual long unsigned int getEventDispatcherId() const;

	//	public virtual constant methods (Component)
	virtual int getAbsoluteX() const;
	virtual int getAbsoluteY() const;
	virtual bool draw( GraphicsContext& aGraphics ) const;

	virtual Dimensions   getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;
	virtual unsigned int getPreferredWidth( const GraphicsContext& gc ) const;
	virtual unsigned int getPreferredHeight( const GraphicsContext& gc ) const;

	//	public virtual constant methods (Drawable)
	virtual       Visual&    getVisual();
	virtual const Visual&    getVisual() const;
	virtual GraphicsContext& getGraphicsContext() const;
	virtual unsigned long    getId() const;
	virtual unsigned long    getScreenId() const;
	virtual unsigned int     getDrawableWidth() const;
	virtual unsigned int     getDrawableHeight() const;
	virtual unsigned int     getDrawableDepth() const;

	//	public abstract constant methods (Panel)
	virtual bool isVisible() const;

	//	public virtual constant methods (Panel)
	virtual Panel*        createChildPanel( flags type ) const;
	virtual Panel*        createSubPanel() const;
	virtual void          setName( const openocl::base::String& aName ) const;
	virtual       Screen& getScreen();
	virtual const Screen& getScreen() const;
	virtual void          flush() const;
	virtual Coordinates queryOuterCoordinates() const;
	virtual PanelGeometry      queryOuterGeometry() const;
	virtual PanelGeometry      queryInnerGeometry() const;


	//	public constant methods
	//void preDraw();
	bool reallyDraw( GraphicsContext& aGraphics ) const;
	void paint( GraphicsContext& onscreen ) const;
	const XVisual& getXVisual() const;
	const XScreen& getXScreen() const;
	XVisual& getXVisual();
	XScreen& getXScreen();


	//virtual void deliverEvent( const openocl::base::event::Event& anEvent );

	virtual void copyArea( const Drawable& aDrawable,
                               unsigned int sourceX,
                               unsigned int sourceY,
                               unsigned int sourceWidth,
                               unsigned int sourceHeight,
                               unsigned int destinationX,
                               unsigned int destinationY ) const;

protected:
	void setHints( int type );
	//void requestXWindowGeometry( int anX,
        //                             int aY,
        //                             unsigned int aWidth,
        //                             unsigned int aHeight,
        //                             unsigned int aBorderWidth );

	void requestXWindowPosition( int x, int y );
	void requestXWindowSize( unsigned int width, unsigned int height );
	void requestXWindowBorder( unsigned int border );



	//void setGeometry(  int anX,
    //                       int aY,  
    //                       unsigned int aWidth,  
    //                       unsigned int aHeight,  
    //                       unsigned int aBorderWidth );

	unsigned long int getWinId() const;

	void calculateAbsoluteCoordinatesOf( openocl::base::event::PointerEvent& pe ) const;

	//	protected members
	long unsigned int winId;
	unsigned int      screen;
	void*             display;
	bool              isMapped;
	XGraphicsContext* gc;

	XPixmap*          buffer;

	unsigned int depth;

private:
	XScreen& xscreen;
	XPanel( XScreen& screen, int type, unsigned long parentId );

	bool fixedSize;
	static const bool BUFFERED = true;
};

};};};

#endif
