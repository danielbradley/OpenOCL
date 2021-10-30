/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_CONTAINER_H
#define OPENOCL_UI_CONTAINER_H

#include "openocl/ui.h"
#include "openocl/ui/Component.h"
#include <openocl/base/event/EventListener.h>
#include <openocl/base/event.h>
#include <openocl/imaging.h>
#include <openocl/util.h>

namespace openocl {
  namespace ui {

class Container : public openocl::ui::Component,
		  public openocl::base::event::EventListener
{
public:
	Container();
	Container( const openocl::base::String& name );
	~Container();

	//	public virtual methods (EventListener)
	virtual void deliverEvent( const openocl::base::event::Event& anEvent );

	//	public virtual methods (Component)
	virtual void setBounds( int x, int y, unsigned int width, unsigned int height );
	virtual void forwardEvent( const openocl::base::event::Event& anEvent );
	virtual void doLayout( const GraphicsContext& gc );
	virtual void setContainingPanel( Panel& aPanel );
	virtual void resetContainingPanel();
	virtual void setName( const openocl::base::String& aName );

	//	public virtual methods (Container)
	virtual void handleActionEvent( const openocl::base::event::ActionEvent& ae );
	void add( Component* aComponent );
	void add( Component& aComponent );
	void remove( Component& aComponent );
	void removeAll();
	void setLayout( LayoutManager* aLayoutManager );
	void setLayout( LayoutManager& aLayoutManager );
	void setBackground( const openocl::imaging::Color& aColor );
	void resetBackground();

	//	public virtual methods
	virtual openocl::util::Sequence& getComponents();

	//	public pseudo-constant methods
	void setRedrawAll( bool value ) const;

	//	public virtual constant methods (Component)
	virtual bool draw( GraphicsContext& gc ) const;
//	virtual unsigned int getPreferredWidth( const GraphicsContext& gc ) const;
//	virtual unsigned int getPreferredHeight( const GraphicsContext& gc ) const;
	virtual Dimensions   getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;

	virtual Region getLayoutRegion() const;

	//	public constant methods
	const openocl::util::Sequence& getComponents() const;
	const openocl::base::String& findValue( const openocl::base::String& componentName ) const
		throw (openocl::base::NoSuchElementException*);

	bool isRedrawAll() const;
protected:
	LayoutManager* layoutManager;

private:
	openocl::util::Sequence* components;
	LayoutManager* deleteableLayoutManager;
	openocl::imaging::Color* background;
	bool redrawAll;
};

};};

#endif
