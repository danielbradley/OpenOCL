/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/Blank.h"
#include "openocl/ui/BorderLayout.h"
#include "openocl/ui/Component.h"
#include "openocl/ui/Container.h"
#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/Region.h"

#include <openocl/base/Math.h>
#include <openocl/base/NoSuchElementException.h>
#include <openocl/util/Sequence.h>

#include <cstdlib>
#include <cstdio>

using namespace openocl::base;
using namespace openocl::util;
using namespace openocl::ui;

BorderLayout::BorderLayout( flags stretch )
{
	this->stretch = stretch;

	this->top    = new Region();
	this->left   = new Region();
	this->right  = new Region();
	this->bottom = new Region();
	this->center = new Region();
}

BorderLayout::~BorderLayout()
{
	delete this->top;
	delete this->left;
	delete this->right;
	delete this->bottom;
	delete this->center;
}

Dimensions
BorderLayout::preferredLayoutDimensions( const GraphicsContext& gc, const Container& aContainer, unsigned int width, unsigned int height ) const
{
	Dimensions d;

	Dimensions top;
	Dimensions left;
	Dimensions right;
	Dimensions bottom;
	Dimensions center;

	const Sequence& components = aContainer.getComponents();

	try
	{
		{
			const Object& obj    = components.elementAt( TOP );
			const Component& com = dynamic_cast<const Component&>( obj );
			top = com.getPreferredDimensions( gc, width, height );
		}
		{
			const Object& obj    = components.elementAt( LEFT );
			const Component& com = dynamic_cast<const Component&>( obj );
			left = com.getPreferredDimensions( gc, width, height - top.height );
		}
		{
			const Object& obj    = components.elementAt( RIGHT );
			const Component& com = dynamic_cast<const Component&>( obj );
			right = com.getPreferredDimensions( gc, width - left.width, height - top.height );
		}
		{
			const Object& obj    = components.elementAt( BOTTOM );
			const Component& com = dynamic_cast<const Component&>( obj );
			bottom = com.getPreferredDimensions( gc, width - (left.width + right.width), height - top.height );
		}
		{
			const Object& obj    = components.elementAt( CENTER );
			const Component& com = dynamic_cast<const Component&>( obj );
			center = com.getPreferredDimensions( gc, width  - (left.width + right.width), height - (top.height + bottom.height) );
		}
	} catch ( NoSuchElementException* ex ) {
		delete ex;
	}

	int side_widths = left.width + right.width;
	int top_bot_heights = top.height + bottom.height;
	if ( center.width > 1 )
	{
		d.width = side_widths + center.width;
		d.height = top_bot_heights + center.height;

		if ( STRETCH_HORIZONTALLY == (STRETCH_HORIZONTALLY & this->stretch) )
		{
#ifdef DEBUG_OPENOCL_UI_BORDERLAYOUT
			fprintf( stderr, "BorderLayout::doLayout: stretching horizontally\n" );
#endif
			d.width  = width - side_widths;
		}

		if ( STRETCH_VERTICALLY == (STRETCH_VERTICALLY & this->stretch) )
		{
#ifdef DEBUG_OPENOCL_UI_BORDERLAYOUT
			fprintf( stderr, "BorderLayout::doLayout: stretching vertically\n" );
#endif
			d.height = height - top_bot_heights;
		}
	} else {
		d.width = side_widths;
	}

	int side_heights = Math::max( left.height, right.height );
	d.height = top_bot_heights + Math::max( side_heights, center.height );

	return d;
}

void
BorderLayout::doLayout( Container& aContainer, const GraphicsContext& gc )
const
{
	Region bounds = aContainer.getBounds();
	
#ifdef DEBUG_OPENOCL_UI_BORDERLAYOUT
	fprintf( stderr, "BorderLayout::doLayout: " );
	bounds.print( stderr );
#endif

	unsigned int width = bounds.width;
	unsigned int height = bounds.height;

	Dimensions top;
	Dimensions left;
	Dimensions right;
	Dimensions bottom;
	Dimensions center;

	Sequence& components = aContainer.getComponents();

	try
	{
		{
			const Object& obj    = components.elementAt( TOP );
			const Component& com = dynamic_cast<const Component&>( obj );
			top = com.getPreferredDimensions( gc, width, height );
		}
		{
			const Object& obj    = components.elementAt( LEFT );
			const Component& com = dynamic_cast<const Component&>( obj );
			left = com.getPreferredDimensions( gc, width, height );
		}
		{
			const Object& obj    = components.elementAt( RIGHT );
			const Component& com = dynamic_cast<const Component&>( obj );
			right = com.getPreferredDimensions( gc, width, height );
		}
		{
			const Object& obj    = components.elementAt( BOTTOM );
			const Component& com = dynamic_cast<const Component&>( obj );
			bottom = com.getPreferredDimensions( gc, width, height );
		}
		{
			const Object& obj    = components.elementAt( CENTER );
			const Component& com = dynamic_cast<const Component&>( obj );
			center = com.getPreferredDimensions( gc, width, height );
		}
	} catch ( NoSuchElementException* ex ) {
		delete ex;
	}

	this->top->x1 = bounds.x1;
	this->top->x2 = bounds.x2;
	this->top->width = this->top->x2 - this->top->x1 + 1;
	this->top->y1 = bounds.y1;
	this->top->height = top.height;
	this->top->y2 = this->top->y1 + top.height - 1;
	
	this->bottom->x1 = bounds.x1;
	this->bottom->x2 = bounds.x2;
	this->bottom->width = bounds.x2 - bounds.x1 + 1;
	this->bottom->y2 = bounds.y2;
	this->bottom->height = bottom.height;
	this->bottom->y1 = this->bottom->y2 - bottom.height + 1;

	this->left->x1 = bounds.x1;
	this->left->width = left.width;
	this->left->x2 = this->left->x1 + this->left->width - 1;
	this->left->y1 = this->top->y2 + 1;
	this->left->y2 = this->bottom->y1 - 1;
	this->left->height = this->left->y2 - this->left->y1 + 1;
	
	this->right->x2 = bounds.x2;
	this->right->width = right.width;
	this->right->x1 = this->right->x2 - this->right->width + 1;
	this->right->y1 = this->top->y2 + 1;
	this->right->y2 = this->bottom->y1 - 1;
	this->right->height = this->right->y2 - this->right->y1 + 1; 

	this->center->x1 = this->left->x2 + 1;
	this->center->x2 = this->right->x1 - 1;
	this->center->width = this->center->x2 - this->center->x1 + 1;
	this->center->y1 = this->top->y2 + 1;
	this->center->y2 = this->bottom->y1 - 1;
	this->center->height = this->center->y2 - this->center->y1 + 1;	 

#ifdef DEBUG_OPENOCL_UI_BORDERLAYOUT
	fprintf( stderr, "\tBorderLayout::doLayout: LEFT:   " );
	this->top->print( stderr );

	fprintf( stderr, "\tBorderLayout::doLayout: RIGHT:  " );
	this->left->print( stderr );

	fprintf( stderr, "\tBorderLayout::doLayout: CENTER: " );
	this->center->print( stderr );

	fprintf( stderr, "\tBorderLayout::doLayout: RIGHT:  " );
	this->right->print( stderr );

	fprintf( stderr, "\tBorderLayout::doLayout: BOTTOM: " );
	this->bottom->print( stderr );
#endif


	try
	{
		{
			Object& obj    = components.elementAt( TOP );
			Component& com = dynamic_cast<Component&>( obj );
			com.setBounds( this->top->x1, this->top->y1, this->top->width, this->top->height );
			com.doLayout( gc );
		}
		{
			Object& obj    = components.elementAt( LEFT );
			Component& com = dynamic_cast<Component&>( obj );
			com.setBounds( this->left->x1, this->left->y1, this->left->width, this->left->height );
			com.doLayout( gc );
		}
		{
			Object& obj    = components.elementAt( RIGHT );
			Component& com = dynamic_cast<Component&>( obj );
			com.setBounds( this->right->x1, this->right->y1, this->right->width, this->right->height );
			com.doLayout( gc );
		}
		{
			Object& obj    = components.elementAt( BOTTOM );
			Component& com = dynamic_cast<Component&>( obj );
			com.setBounds( this->bottom->x1, this->bottom->y1, this->bottom->width, this->bottom->height );
			com.doLayout( gc );
		}
		{
			Object& obj    = components.elementAt( CENTER );
			Component& com = dynamic_cast<Component&>( obj );
			com.setBounds( this->center->x1, this->center->y1, this->center->width, this->center->height );
			com.doLayout( gc );
		}
	} catch ( NoSuchElementException* ex ) {
		fprintf( stderr, "BorderLayout::doLayout(): XXXXXXXXXXXXXXXX exception\n" );
		delete ex;
	}


	//fprintf( stderr, "BorderLayout needs to be reimplemented! ABORTED\n" );
	//abort();








/*
	const Sequence& components = aContainer.getComponents();

	unsigned int x = aContainer.getX();
	unsigned int y = aContainer.getY();
	unsigned int w = aContainer.getWidth();
	unsigned int h = aContainer.getHeight();

	unsigned int north_border = 0;
	unsigned int south_border = y + h;
	unsigned int west_border = 0;
	unsigned int east_border = x + w;

	try
	{
		north_border = y;
		west_border  = x;
		east_border  = w;
		south_border = h;
	
	components.elementAt( 0 );
		Component& north = const_cast<Component&>( dynamic_cast<const Component&>( components.elementAt( 0 ) ) );
		if ( &north )
		{
			north_border = y + Math::min( north.getPreferredDimensions( gc, *this, w, h ).height, h );
			north.setBounds( x, y, w, north_border );
			north.doLayout( gc );
		}
		
		Component& west = const_cast<Component&>( dynamic_cast<const Component&>( components.elementAt( 1 ) ) );
		if ( &west )
		{
			west_border = x + Math::min( west.getPreferredWidth( gc ), w );
			west.setBounds( x, north_border, west_border - x, south_border - north_border );
			west.doLayout( gc );
		}

		Component& east = const_cast<Component&>( dynamic_cast<const Component&>( components.elementAt( 3 ) ) );
		if ( &east )
		{
			east_border = Math::max( x + (w - east.getPreferredWidth( gc )), west_border );
			east.setBounds( east_border, north_border, w - ( east_border - x), south_border - north_border );
			east.doLayout( gc );
		}
		
		Component& south = const_cast<Component&>( dynamic_cast<const Component&>( components.elementAt( 4 ) ) );
		if ( &south )
		{
			south_border = Math::max( y + (h - south.getPreferredDimensions( gc, *this, w, h ).height), north_border );
			south.setBounds( x, south_border, w, h - (south_border - y) );
			south.doLayout( gc );

			if ( &west )
			{
				west.setBounds( x, north_border, west_border - x, south_border - north_border );
				west.doLayout( gc );
			}
			if ( &east )
			{
				east.setBounds( east_border, north_border, w - ( east_border - x), south_border - north_border );
				east.doLayout( gc );
			}
		}
		
		Component& centre = const_cast<Component&>( dynamic_cast<const Component&>( components.elementAt( 2 ) ) );
		if ( &centre )
		{
			centre.setBounds( west_border, north_border, east_border - west_border, south_border - north_border );
		}
		
	} catch ( Exception* ex ) {
		delete ex;
	}
*/
/*
	unsigned int north_border = min(
	unsigned int west_border

	try
	{
		Component& north = (Component&) aContainer.elementAt( 0 );
		unsigned int north_w = w;
		unsigned int north_h = min( north.getPreferredHeight( gc ), h );
		north.setBounds( x, y, north_w, north_h );

		Component& west = (Component&) aContainer.elementAt( 1 );
		unsigned int west_w = w;
		unsigned int west_h = min( comp0.getPreferredHeight( gc ), h );
		comp0.setBounds( x, y, width0, height0 );



	}


    y += height0;
    h -= height0;

    Component& comp1 = (Component&) aContainer.elementAt( 1 );
    unsigned int width1 = min( comp1.getPreferredWidth( gc ), w );
    unsigned int height1 = min( comp1.getPreferredHeight( gc ), h );
    comp1.setBounds( x, y, width1, height1 );

    x += width1;
    w -= width1;
    y += height1;
    h -= height1;

    Component& comp2 = (Component&) aContainer.elementAt( 2 );
    unsigned int width2 = min( comp2.getPreferredWidth( gc ), w );
    unsigned int height2 = min( comp2.getPreferredHeight( gc ), h );
    w -= width2;
    comp2.setBounds( w, y, width2, height2 );

    Component& comp3 = (Component&) aContainer.elementAt( 3 );
    unsigned int height3 = min( comp2.getPreferredHeight( gc ), h );




    x += width2;
    w -= width2;
    y += height2;
    h -= height2;
  */
}
