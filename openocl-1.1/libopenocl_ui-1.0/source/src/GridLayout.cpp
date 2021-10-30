/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/GridLayout.h"

#include <openocl/base/Debug.h>
#include <openocl/base/Iterator.h>
#include <openocl/base/Math.h>
#include <openocl/base/Runtime.h>
#include <openocl/base/String.h>
#include <openocl/base/StringBuffer.h>
#include <openocl/io/IO.h>
#include <openocl/io/Printer.h>
#include "openocl/ui/Component.h"
#include "openocl/ui/Container.h"
#include "openocl/ui/GraphicsContext.h"
#include <openocl/util/Sequence.h>
#include <openocl/util/Stack.h>

#include <cstdlib>
#include <cstdio>

using namespace openocl::base;
using namespace openocl::io;
using namespace openocl::ui;
using namespace openocl::util;

GridLayout::GridLayout( unsigned int nrOfColumns ) : LayoutManager(), cols( nrOfColumns )
{
	this->hints = 0;
	this->hints |= LayoutManager::UPWARD;
	this->hints |= LayoutManager::EXPANDED;
}

GridLayout::GridLayout( unsigned int nrOfColumns, flags layoutHints ) : cols( nrOfColumns )
{
	this->hints = layoutHints;
}

GridLayout::~GridLayout()
{}

void
GridLayout::doLayout( Container& aContainer, const GraphicsContext& gc ) const
{

	fprintf( stderr, "GridLayout needs to be reimplemented! ABORTED\n" );
	abort();

/*
	const Sequence& components = dynamic_cast<const Sequence&>( aContainer.getComponents() );

	unsigned int x = aContainer.getX();
	unsigned int y = aContainer.getY();
	unsigned int container_width  = aContainer.getWidth();
	unsigned int container_height = aContainer.getHeight();

	unsigned int nr_of_cells = components.getLength();
	unsigned int nr_of_rows  = (unsigned int) Math::roof( (nr_of_cells*1.0) / (this->cols*1.0) );
	
	unsigned int* widths  = (unsigned int*) Runtime::calloc( nr_of_cells, sizeof( unsigned int ) );
	unsigned int* heights = (unsigned int*) Runtime::calloc( nr_of_cells, sizeof( unsigned int ) );

	for ( unsigned int i=0; i < nr_of_cells; i++ )
	{
		widths[i] = 0;
		heights[i] = 0;
		try
		{
			Component& com = const_cast<Component&>( dynamic_cast<const Component&>( components.elementAt( i ) ) );
			widths[i] = com.getPreferredWidth( gc );
			heights[i] = com.getPreferredHeight( gc );
		} catch ( Exception* ex ) {
			delete ex;
		}
	}
	
	for ( unsigned int row=0; row < nr_of_rows; row++ )
	{
		unsigned int largest_height = 0;
		for ( unsigned int col=0; col < this->cols; col++ )
		{
			unsigned int cell_nr = (row*this->cols) + col;
			largest_height = Math::max( largest_height, heights[cell_nr] );
		}
		for ( unsigned int col=0; col < this->cols; col++ )
		{
			unsigned int cell_nr = (row*this->cols) + col;
			heights[cell_nr] = largest_height;
		}
	}

	for ( unsigned int col=0; col < this->cols; col++ )
	{
		unsigned int largest_width = 0;
		for ( unsigned int row=0; row < nr_of_rows; row++ )
		{
			unsigned int cell_nr = (row*this->cols) + col;
			largest_width = Math::max( largest_width, widths[cell_nr] );
		}
		for ( unsigned int row=0; row < nr_of_rows; row++ )
		{
			unsigned int cell_nr = (row*this->cols) + col;
			widths[cell_nr] = largest_width;
		}
	}

	unsigned int offset_y = y;
	for ( unsigned int row=0; row < nr_of_rows; row++ )
	{
		unsigned int offset_x = x;
		for ( unsigned int col=0; col < this->cols; col++ )
		{
			unsigned int cell_nr = (row*this->cols) + col;
			try
			{
				unsigned int width  = Math::min( widths[cell_nr], container_width - (offset_x - x) );
				unsigned int height = Math::min( heights[cell_nr], container_height - (offset_y - y) );
			
				Component& com = const_cast<Component&>( dynamic_cast<const Component&>( components.elementAt( cell_nr ) ) );
				com.setBounds( offset_x, offset_y, width, height );
				//IO::err().printf( "GridLayout::doLayout: %s.com.setBounds( %u, %u, %u, %u )\n", aContainer.getName().getChars(), offset_x, offset_y, width, height );
				com.doLayout( gc );
				offset_x += width;
			} catch ( Exception* ex ) {
				delete ex;
			}
		}
		offset_y += heights[row*this->cols];
	}

	Runtime::free( widths );
	Runtime::free( heights );
*/
/*(
	//IO::err().printf( "ColumnLayout::doLayout( Container&, GraphicsContext )\n" );

	unsigned int x = aContainer.getX();
	unsigned int y = aContainer.getY();
	unsigned int containerWidth = aContainer.getWidth();
	unsigned int containerHeight = aContainer.getHeight();

	unsigned int offset = 0;

	Iterator* it = aContainer.getComponents().iterator();
	
	switch ( this->direction )
	{
	case WEST:
		{
			Stack stack;
			while ( it->hasNext() )
			{
				Object* obj = const_cast<Object*>( &it->next() ); 
				stack.push( obj );
			}
			
			while ( stack.getSize() )
			{
				Component* com = dynamic_cast<Component*>( stack.pop() );
				
				unsigned int width;
				unsigned int height = containerHeight;
				
				if ( stack.getSize() )
				{
					width = Math::min( com->getPreferredWidth( gc ), containerWidth - offset );
				} else {
					width = containerWidth - offset;
				}
				offset = offset + width;
			
				com->setBounds( (x + containerWidth) - offset, y, width, height );
				com->doLayout( gc );
				
			}
		}
		break;
	case EAST:	//	drop through to default
	default:
		{
			while ( it->hasNext() )
			{
				Component& com = const_cast<Component&>( dynamic_cast<const Component&>( it->next() ) );

				unsigned int width;
				unsigned int height = containerHeight;

				if ( it->hasNext() )
				{
					width = Math::min( com.getPreferredWidth( gc ), containerWidth - offset );
				} else {
					width = containerWidth - offset;
				}

				com.setBounds( x + offset, y, width, height );
				com.doLayout( gc );

				offset = offset + width;
			}
		}
		break;
	};
	delete it;
*/
}

/**
 *  This is called before the containers bounds are set.
 */
/*
unsigned int
GridLayout::preferredLayoutWidth( const Sequence& components, const GraphicsContext& gc ) const
{
	unsigned int preferred_width = 0;

	unsigned int nr_of_cells = components.getLength();
	unsigned int nr_of_rows  = (unsigned int) Math::roof( (nr_of_cells*1.0) / (this->cols*1.0) );
	
	for ( unsigned int row=0; row < nr_of_rows; row++ )
	{
		unsigned int row_width = 0;
		try
		{
			for ( unsigned int i=0; i < this->cols; i++ )
			{
				unsigned int cell_nr = (row * this->cols) + i;
				const Component& com = dynamic_cast<const Component&>( components.elementAt( cell_nr ) );
				row_width += com.getPreferredWidth( gc );
			}
		} catch ( Exception* ex ) {
			delete ex;
		}
		preferred_width = Math::max( preferred_width, row_width );
	}
	return preferred_width;
}

unsigned int
GridLayout::preferredLayoutHeight( const Sequence& components, const GraphicsContext& gc ) const
{
	unsigned int preferred_height = 0;

	unsigned int nr_of_cells = components.getLength();
	unsigned int nr_of_rows  = (unsigned int) Math::roof( (nr_of_cells*1.0) / (this->cols*1.0) );
	
	for ( unsigned int i=0; i < this->cols; i++ )
	{
		unsigned int col_height = 0;
		try
		{
			for ( unsigned int row=0; row < nr_of_rows; row++ )
			{
				unsigned int cell_nr = (row * this->cols) + i;
				const Component& com = dynamic_cast<const Component&>( components.elementAt( cell_nr ) );
				col_height += com.getPreferredHeight( gc );
			}
		} catch ( Exception* ex ) {
			delete ex;
		}
		preferred_height = Math::max( preferred_height, col_height );
	}

	return preferred_height;
}
*/
Dimensions
GridLayout::preferredLayoutDimensions( const GraphicsContext& gc, const Container& aContainer, unsigned int width, unsigned int height ) const
{
	Dimensions dim;
	dim.width = 0xFFFF;
	dim.height = 0xFFFF;
	return dim;
}
