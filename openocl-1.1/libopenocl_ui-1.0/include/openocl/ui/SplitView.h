/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_SPLITVIEW_H
#define OPENOCL_UI_SPLITVIEW_H

#include "openocl/ui/Container.h"
#include "openocl/ui/LayoutManager.h"
#include "openocl/ui.h"
#include <openocl/base.h>
#include <openocl/imaging.h>
#include <openocl/util.h>

namespace openocl {
	namespace ui {

class SplitView : public Container, public LayoutManager
{
public:
	static unsigned int gapSize;

	SplitView( flags layoutHints );
	virtual ~SplitView();

	//	public virtual methods (Component)
	//virtual void doLayout( const GraphicsContext& gc );
	virtual void processEvent( const openocl::base::event::Event& anEvent );

	//	public virtual methods (Container)
	virtual void add( Component& component );
	virtual void add( Component* component );

	//	public methods
	void nextComponent();

	//	public virtual const methods (Component)
	bool draw( GraphicsContext& gc ) const;

	//	public virtual const methods (LayoutManager)
	virtual void doLayout( Container& aContainer, const GraphicsContext& gc ) const;
	virtual Dimensions preferredLayoutDimensions( const GraphicsContext& gc, const Container& aContainer, unsigned int width, unsigned int height ) const;
	//virtual unsigned int preferredLayoutWidth( const openocl::util::Sequence& components, const GraphicsContext& gc ) const;
	//virtual unsigned int preferredLayoutHeight( const openocl::util::Sequence& components, const GraphicsContext& gc ) const;

private:
	flags hints;
	unsigned int nrOfComponents;

	//	Depending on size of Container
	int privilegedWidth;

	Region* outline;
	Region* gap;
	Region* fixed;
	Region* sub;
	Region* handle;
	
	bool click;
	int selectedCell;
	int clickX;
	int clickY;
	int clickWidth;
};

};};

#endif
