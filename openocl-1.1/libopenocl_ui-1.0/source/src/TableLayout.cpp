/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/base/Math.h"
#include "openocl/base/Runtime.h"
#include "openocl/ui/Component.h"
#include "openocl/ui/Container.h"
#include "openocl/ui/TableLayout.h"
#include "openocl/util/Sequence.h"

#include <cstdio>

using namespace openocl::base;
using namespace openocl::ui;
using namespace openocl::util;

TableLayout::TableLayout( unsigned int nrOfColumns )
{
	this->columns = nrOfColumns;
	this->minWidths = (unsigned int*) Runtime::calloc( nrOfColumns, sizeof( unsigned int ) );
	this->commonHeight = 0;
}

TableLayout::~TableLayout()
{
	Runtime::free( this->minWidths );
}

Dimensions
TableLayout::preferredLayoutDimensions( const GraphicsContext& gc, const Container& aContainer, unsigned int width, unsigned int height ) const
{
	return preferredLayoutEastward( gc, aContainer, width, height );

	//fprintf( stderr, "TableLayout::preferredLayoutDimensions: %u %u\n", width, height );

	const_cast<TableLayout*>( this )->commonHeight = 0;
	for ( unsigned int i=0; i < this->columns; i++ )
	{
		this->minWidths[i] = 0;
	}


	const Sequence& s = aContainer.getComponents();
	unsigned int nr = s.getLength();

	unsigned int nr_of_rows = nr/this->columns;
	if ( 0 != (nr % this->columns) ) nr_of_rows++;

	int row_height = 0;

	Dimensions d;

	try
	{
		int remaining_height = height;
		int remaining_width = width;
		for ( unsigned int r=0; r < nr_of_rows; r++ )
		{
			row_height = 0;
			remaining_width = width;
			for ( unsigned int c=0; c < this->columns; c++ )
			{
				const Object& obj = s.elementAt( (r * this->columns) + c );
				const Component& com = dynamic_cast<const Component&>( obj );
				Dimensions dim = com.getPreferredDimensions( gc, remaining_width, remaining_height );
				this->minWidths[c] = Math::max( this->minWidths[c], dim.width );
				remaining_width -= this->minWidths[c];
				row_height = Math::max( row_height, dim.height );
				const_cast<TableLayout*>( this )->commonHeight = Math::max( this->commonHeight, row_height );
			}
			
			remaining_height -= row_height;
		}
	} catch ( IndexOutOfBoundsException* ex ) {
	
		//	Most likely this will always be thrown as the above bound
		//	for r is incorrect.
		delete ex;
	}

	d.height = nr_of_rows * this->commonHeight;			

	for ( unsigned int i=0; i < this->columns; i++ )
	{
		//fprintf( stderr, "TableLayout::preferredLayoutDimensions: %i: %i\n", i, this->minWidths[i] );
		d.width += this->minWidths[i];
	}

	//	Testing
	if ( true )
	{
		d.width = width;
		int max = this->columns;
		int col = width / this->columns;
		for ( int i=0; i < max; i++ )
		{
			this->minWidths[i] = col;
		}
	}

	return d;
}

Dimensions
TableLayout::preferredLayoutEastward( const GraphicsContext& gc, const Container& aContainer, unsigned int width, unsigned int height ) const
{
	//fprintf( stderr, "TableLayout::preferredLayoutEastward: %u %u\n", width, height );

	const_cast<TableLayout*>( this )->commonHeight = 0;
	for ( unsigned int i=0; i < this->columns; i++ )
	{
		this->minWidths[i] = 0;
	}

	const Sequence& s = aContainer.getComponents();
	unsigned int max = s.getLength();

	int nr_of_rows = max/this->columns;
	if ( 0 != (max % this->columns) ) nr_of_rows++;

	try
	{
		int row_height = 0;
		int remaining_width = width;
		int max = this->columns;

		for ( int col=0; col < max; col++ )
		{
			int remaining_height = height;
			for ( int row=0; row < nr_of_rows; row++ )
			{
				const Object& obj = s.elementAt( (row * this->columns) + col );
				const Component& com = dynamic_cast<const Component&>( obj );
				Dimensions dim = com.getPreferredDimensions( gc, remaining_width, remaining_height );
				this->minWidths[col] = Math::max( this->minWidths[col], dim.width );
				row_height = Math::max( row_height, dim.height );
				remaining_height -= row_height;
				const_cast<TableLayout*>( this )->commonHeight = Math::max( this->commonHeight, row_height );
			}
			remaining_width -= this->minWidths[col];
		}
	} catch ( IndexOutOfBoundsException* ex ) {
	
		//	Most likely this will always be thrown as the above bound
		//	for r is incorrect.
		delete ex;
	}

	Dimensions d;
	d.height = nr_of_rows * this->commonHeight;			

	for ( unsigned int i=0; i < this->columns; i++ )
	{
		//fprintf( stderr, "TableLayout::preferredLayoutDimensions: %i: %i\n", i, this->minWidths[i] );
		d.width += this->minWidths[i];
	}

	//	Testing
	if ( false )
	{
		d.width = width;
		int col = width / this->columns;
		int max = this->columns;
		for ( int i=0; i < max; i++ )
		{
			this->minWidths[i] = col;
		}
	}

	return d;
}

void
TableLayout::doLayout( Container& aContainer, const GraphicsContext& gc ) const
{
	Sequence& s = aContainer.getComponents();
	unsigned int max = s.getLength();

	Region layout_region = aContainer.getContentRegion();
	
	int x  = layout_region.x1;
	int dx = x;
	int dy = layout_region.y1;

	try
	{
		for ( unsigned int r=0; r < max; r++ )
		{
			dx = x;
			for ( unsigned int c=0; c < this->columns; c++ )
			{
				Object& obj = s.elementAt( (r * this->columns) + c );
				Component& com = dynamic_cast<Component&>( obj );
				com.setBounds( dx, dy, this->minWidths[c], this->commonHeight );
				com.doLayout( gc );
				//fprintf( stderr, "TableLayout::doLayout: %i %i %u %u\n", dx, dy, this->minWidths[c], this->commonHeight );
				
				dx += this->minWidths[c];
			}
			dy += this->commonHeight;
		}
	} catch ( IndexOutOfBoundsException* ex ) {
		delete ex;
	}
}
