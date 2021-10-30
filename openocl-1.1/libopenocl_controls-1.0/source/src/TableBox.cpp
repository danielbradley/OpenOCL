/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/controls/TableBox.h"
#include "openocl/base/FormattedString.h"
#include <openocl/base/Iterator.h>
#include <openocl/base/Math.h>
#include <openocl/base/String.h>
#include <openocl/base/StringBuffer.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/ChangeEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/KeyEvent.h>
#include <openocl/base/event/MouseEvent.h>
#include <openocl/data/HeaderTuple.h>
#include <openocl/data/TableData.h>
#include <openocl/data/Tuple.h>
#include <openocl/imaging/Color.h>
#include <openocl/imaging/TextExtents.h>
#include <openocl/ui/Canvas.h>
#include <openocl/ui/Container.h>
#include <openocl/ui/GraphicsContext.h>
#include <openocl/ui/Icon.h>
#include <openocl/ui/IconGroup.h>
#include <openocl/ui/Label.h>
#include <openocl/ui/OffscreenImageManager.h>
#include <openocl/ui/OpaqueContainer.h>
#include <openocl/ui/Panel.h>
#include <openocl/ui/Region.h>
#include <openocl/ui/TableLayout.h>
#include <openocl/ui/TextField.h>
#include <openocl/ui/SidewaysLayout.h>
#include <openocl/ui/Scroller.h>
#include <openocl/ui/ScrollPane.h>
#include <openocl/ui/Visual.h>
#include <openocl/util/Sequence.h>

#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::controls;
using namespace openocl::data;
using namespace openocl::imaging;
using namespace openocl::ui;
using namespace openocl::util;

namespace openocl {
	namespace controls {
		namespace tableboxcontrol {

class ColumnWidths : public Object
{
public:
	ColumnWidths()
	{
		this->widths = null;
		this->nrOfColumns = 0;
	}
	
	~ColumnWidths()
	{
		delete this->widths;
	}

	void init( unsigned int n )
	{
		this->nrOfColumns = n;
		this->widths = new int[n];
	}

	unsigned int sum() const
	{
		unsigned int sum = 0;
		unsigned int max = this->nrOfColumns;
		
		if ( this->widths )
		{
			for ( unsigned int i=0; i < max; i++ )
			{
				sum += this->widths[i];
			}
		}
		return sum;
	}

	int*         widths;
	unsigned int nrOfColumns;
};

class HeaderCanvas : public openocl::ui::Canvas
{
public:
	HeaderCanvas( TableData& data, ColumnWidths& columnWidths ) : Canvas(), data( data ), columnWidths( columnWidths )
	{
		this->tupleHeader = null;

		this->setPadding( 5 );
		this->textAscent = 0;
		this->textDescent = 0;
		this->initialized = false;
		this->valid = false;
		this->scrollOffset = 0;
	}
	
	~HeaderCanvas()
	{
		delete this->tupleHeader;
	}

	void setScrollOffset( unsigned int value )
	{
		this->scrollOffset = value;
	}

	Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned height ) const
	{
		Dimensions d;
		unsigned int pad = this->getPadding() * 2;
		if ( !this->initialized && this->valid )
		{
			const_cast<HeaderCanvas*>( this )->tupleHeader = data.headerTuple();
			unsigned int max = data.getNrOfCols();
			
			if ( !this->columnWidths.widths )
			{
				this->columnWidths.init( data.getNrOfCols() );
			}
			
			for ( unsigned int i=0; i < max; i++ )
			{
				String* field_name = this->tupleHeader->fieldName( i );
				TextExtents* text_extents = gc.measureTextExtents( *field_name );
				{
					this->columnWidths.widths[i] = Math::max( this->columnWidths.widths[i], text_extents->width + pad );
					const_cast<HeaderCanvas*>( this )->textAscent  = Math::max( this->textAscent,  text_extents->ascent );
					const_cast<HeaderCanvas*>( this )->textDescent = Math::max( this->textDescent, text_extents->descent );
				}
				delete field_name;
				delete text_extents;
			}
			d.width = pad;
			const_cast<HeaderCanvas*>( this )->initialized = true;
		}
		if ( this->valid )
		{
			d.width += this->columnWidths.sum();
			d.height = this->textAscent + this->textDescent + pad + pad;
		}
		return d;
	}

	bool drawCanvas( GraphicsContext& gc ) const
	{
			unsigned int padding = this->getPadding();
		
			Region bounds = this->getBounds();
			Region box    = bounds.shrunkBy( padding );
			Region clip   = box.shrunkBy( 1 );
			Region field  = clip;
		
			int offset = bounds.x1 + padding - this->scrollOffset;
			int y = bounds.y2 - padding - padding - this->textDescent;

			gc.setFill( Color::DIALOG );
			gc.drawFilledRectangleRegion( bounds );
			this->drawOutward3DBox( gc, bounds, Color::SHINE2, Color::SHADE1 );
			this->drawInward3DBox( gc, box, Color::SHINE2, Color::SHADE1 );
		
		if ( this->tupleHeader )
		{
			gc.setClip( clip.x1, clip.y1, clip.width, clip.height );
			unsigned int max = data.getNrOfCols();
			for ( unsigned int i=0; i < max; i++ )
			{
				String* field_name = this->tupleHeader->fieldName( i );
				TextExtents* text_extents = gc.measureTextExtents( *field_name );
				{
					field.x1 = offset + 1;
					offset += padding;
	
					gc.setForeground( Color::DARK_GREY );
					gc.drawString( *field_name, offset, y );

					offset += this->columnWidths.widths[i];
					offset += padding;
					field.x2 = offset;
					field.width = field.x2 - field.x1 + 1;

					this->drawOutward3DBox( gc, field, Color::SHINE2, Color::SHADE1 );
				}
				delete field_name;
				delete text_extents;
			}
			gc.popClip();
		}
		return false;
	}

	void refresh()
	{
		if ( this->data.getNrOfRows() && this->data.getNrOfCols() )
		{
			this->valid = true;
			this->fireChangeEvent();
		}
	}
		
	TableData&   data;
	ColumnWidths& columnWidths;

	Tuple*       tupleHeader;
	unsigned int textAscent;
	unsigned int textDescent;
	bool         valid;
	bool         initialized;
	
	unsigned int scrollOffset;
};

class TableCanvas : public openocl::ui::Canvas
{
public:
	TableCanvas( TableData& data, ColumnWidths& columnWidths ) : Canvas(), data( data ), columnWidths( columnWidths ), rows( 0 ), cols( 0 )
	{
		this->tuples = null;
		this->textHeight = 0;
		this->textDescent = 0;
		this->rowHeight = 0;
		this->initialized = false;
		
		this->setPadding( 5 );
		this->selectedRow = -1;
	}
	
	~TableCanvas()
	{
		delete this->tuples;
	}

	void processEvent( const Event& anEvent )
	{
		Region bounds = this->getBounds();
		bool fire_event = false;
		bool fire_change = false;
	
		flags type = anEvent.getType();
		switch ( type )
		{
		case Event::MOUSE_EVENT:
			{
				const MouseEvent& me = dynamic_cast<const MouseEvent&>( anEvent );
				int x = me.getX();
				int y = me.getY();
				if ( this->contains( x, y ) )
				{
					if ( MouseEvent::BUTTON1_PRESS == (MouseEvent::BUTTON1_PRESS & me.getButton()) )
					{
						int offset = y - bounds.y1;
						int next_selected_row = offset / this->rowHeight;
						
						if ( me.getClickCount() > 1 )
						{
							this->selectedRow = next_selected_row;
							fire_event = true;
						}
						else if ( this->selectedRow != next_selected_row )
						{
							this->selectedRow = next_selected_row;
							fire_change = true;
						}
						else
						{
							this->selectedRow = -1;
							fire_change = true;
						}
						
						//fprintf( stderr, "TableBox::TableCanvas::processEvent: selectedRow: %i\n", this->selectedRow );
					}
				}
			}
		}
		
		if ( fire_event && ( -1 != this->selectedRow ) )
		{
			const Tuple& tuple = dynamic_cast<const Tuple&>( this->tuples->elementAt( this->selectedRow ) );
			unsigned int max = tuple.getNrOfFields();
			StringBuffer sb;
			sb.append( "<tuple" );
			for ( unsigned int i=0; i < max; i++ )
			{
				String* fieldName = tuple.fieldName( i );
				String* value     = tuple.valueAt( i );
				FormattedString str( " %s=\"%s\"", fieldName->getChars(), value->getChars() );
				sb.append( str );
			}
			sb.append( "/>" );

			ActionEvent ae( *this, 0, "TUPLE.openocl::controls::TableBox.DOUBLE_CLICK", sb.asString() );
			this->fireEvent( ae );
		}
		
		if ( fire_change )
		{
			this->fireChangeEvent();
			this->getContainingPanel().redraw();
		}
	}

	void refresh()
	{
		//fprintf( stderr, "TableContainer::refresh()\n" );
		this->rows = this->data.getNrOfRows();
		this->cols = this->data.getNrOfCols();

		if ( this->rows && this->cols )
		{
			delete this->tuples;
			this->tuples = new Sequence();
	
			for ( unsigned int r=0; r < rows; r++ )
			{
				Tuple* tuple = data.tupleAt( r );
				Object* obj = dynamic_cast<Object*>( tuple );
				if ( obj )
				{
					this->tuples->add( obj );
				}
			}
			this->fireChangeEvent();
			this->getContainingPanel().relayout();
		}
	}

	Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned height ) const
	{
		Dimensions d;

		const int pad = this->getPadding() * 2;
	
		if ( !this->initialized && this->rows && this->cols )
		{
			const Tuple& tuple = dynamic_cast<const Tuple&>( this->tuples->elementAt( 0 ) );
			unsigned int rows = this->data.getNrOfRows();
			unsigned int cols = tuple.getNrOfFields();
			
			if ( !this->columnWidths.widths )
			{
				const_cast<TableCanvas*>( this )->columnWidths.init( cols );
			}
			
			for ( unsigned int i=0; i < rows; i++ )
			{
				const Tuple& tuple = dynamic_cast<const Tuple&>( this->tuples->elementAt( i ) );
				for ( unsigned int j=0; j < cols; j++ )
				{
					String* field = tuple.valueAt( j );
					TextExtents* text_extents = gc.measureTextExtents( *field );
					
					this->columnWidths.widths[j] = Math::max( j, gc.measureTextWidth( *field ) + pad );
					const_cast<TableCanvas*>( this )->textHeight  = Math::max( this->textHeight, text_extents->ascent );
					const_cast<TableCanvas*>( this )->textDescent = Math::max( this->textDescent, text_extents->descent );

					delete field;
					delete text_extents;
				}
			}
			
			const_cast<TableCanvas*>( this )->preferredWidth = 0;
			for ( unsigned int j=0; j < cols; j++ )
			{
				const_cast<TableCanvas*>( this )->preferredWidth += this->columnWidths.widths[j];
				const_cast<TableCanvas*>( this )->preferredWidth += pad;
			}
			
			const_cast<TableCanvas*>( this )->rowHeight = this->textHeight + this->textDescent + pad;
			const_cast<TableCanvas*>( this )->preferredHeight = rows * this->rowHeight;
			const_cast<TableCanvas*>( this )->initialized = true;
			const_cast<TableCanvas*>( this )->setLineHeight( this->rowHeight );
		}
		
		d.width  = this->preferredWidth;
		d.height = this->preferredHeight;
		return d;
	}

	int getOffsetOfRow( int row ) const
	{
		int offset = this->getPadding() * 2;
		offset += this->textHeight + this->textDescent;
		offset *= row;
		return offset;
	}

	bool drawCanvas( GraphicsContext& gc ) const
	{
		if ( this->columnWidths.widths )
		{
			Color original_background( gc.getBackground() );
			gc.setBackground( Color::WHITE );

			const int padding = this->getPadding();
			const int pad = padding * 2;
			const int base_offset = padding + this->textDescent;

			Region tuple_region;
			tuple_region.x1 = 0;
			tuple_region.width = this->preferredWidth;
			tuple_region.x2 = tuple_region.x1 + tuple_region.width - 1;
			tuple_region.y1 = 0;
			tuple_region.height = this->textHeight + this->textDescent + pad;
			tuple_region.y2 = tuple_region.y1 + tuple_region.height - 1;

			bool begun = false;
			int rows = this->rows;
			unsigned int cols = this->cols;
			for ( int i=0; i < rows; i++ )
			{
				if ( gc.canDraw( tuple_region ) )
				{
					begun = true;

					if ( i == this->selectedRow )
					{
						Region selected_region = tuple_region;
						selected_region.y1 = this->selectedRow * this->rowHeight;
						selected_region.height = this->rowHeight;
						selected_region.y2 = selected_region.y1 + selected_region.height - 1;
						gc.setFill( Color::BLUE );
						gc.drawFilledRectangleRegion( selected_region );
					}

					//this->drawInward3DBox( gc, tuple_region, Color::SHINE2, Color::SHADE1 );
					gc.setForeground( Color::LIGHT_GREY );
					gc.drawLine( tuple_region.x1, tuple_region.y1, tuple_region.x2, tuple_region.y1 );
				
					const Tuple& tuple = dynamic_cast<const Tuple&>( this->tuples->elementAt( i ) );
					int offset = 0;
					for ( unsigned int j=0; j < cols; j++ )
					{
						String* value = tuple.valueAt( j );
						offset += padding;

						if ( i == this->selectedRow )
						{
							gc.setForeground( Color::WHITE );
							gc.setBackground( Color::BLUE );
							gc.drawString( *value, offset, tuple_region.y2 - base_offset );
							gc.setBackground( Color::WHITE );
						} else {
							gc.setForeground( Color::DARK_GREY );
							gc.drawString( *value, offset, tuple_region.y2 - base_offset );
						}
						delete value;
						offset += this->columnWidths.widths[j];
						offset += padding;
						
						gc.setForeground( Color::LIGHT_GREY );
						gc.drawLine( offset, tuple_region.y1, offset, tuple_region.y2 );
					}
				} else {
					if ( begun ) break;
				}
			
				tuple_region.y1 += tuple_region.height;
				tuple_region.y2 += tuple_region.height;
			}
			gc.setBackground( original_background );
		}
		return false;
	}

private:
	int textHeight;
	int textDescent;
	int rowHeight;

	TableData&   data;
	ColumnWidths& columnWidths;
	unsigned int rows;
	unsigned int cols;

	Sequence* tuples;

	unsigned int preferredWidth;
	unsigned int preferredHeight;

	bool initialized;
	
	int selectedRow;
};

class Base : public openocl::ui::Container
{
public:
	Base( TableData& data )
	: Container(), data( data ), headerCanvas( data, this->columnWidths ), tableCanvas( data, this->columnWidths )
	{
		this->scrollPane = new ScrollPane( this->tableCanvas );
		this->add( this->headerCanvas );
		this->add( *this->scrollPane );
	}
	
	~Base()
	{
		this->remove( this->headerCanvas );
		this->remove( *this->scrollPane );
		delete this->scrollPane;
	}
	
	void refresh()
	{
		this->headerCanvas.refresh();
		this->tableCanvas.refresh();
	}
	
	TableData& data;
	ColumnWidths columnWidths;
	HeaderCanvas headerCanvas;
	TableCanvas  tableCanvas;
	ScrollPane*  scrollPane;
};

};};};

TableBox::TableBox( TableData& data )
: Control( "openocl::controls::TableBox" )
{
	this->base = new openocl::controls::tableboxcontrol::Base( data );
	this->setControlComponent( *this->base );
	this->base->addEventListener( *this );
}

TableBox::~TableBox()
{
	this->base->removeEventListener( *this );
	delete this->base;
}

//--------------------------------------------
//	public virtual methods (EventListener)
//--------------------------------------------

bool
TableBox::handleActionEvent( const ActionEvent& anActionEvent )
{
	const String& command = anActionEvent.getCommand();
	const String& data    = anActionEvent.getData();

	//fprintf( stderr, "TableBox::handleActionEvent: command=\"%s\" data=\"%s\"\n", command.getChars(), data.getChars() );
	
	if ( command.startsWith( "HSCROLL." ) )
	{
		int value = data.getValueAsInt();
		dynamic_cast<openocl::controls::tableboxcontrol::Base*>( this->base )->headerCanvas.setScrollOffset( value );
	}
	return false;
}

void
TableBox::refresh()
{
	ChangeEvent ce( *this, 0, ChangeEvent::REDRAW_ALL );
	dynamic_cast<openocl::controls::tableboxcontrol::Base*>( this->base )->refresh();
	this->base->forwardEvent( ce );
}

void
TableBox::selectRow( int row )
{
	//dynamic_cast<openocl::controls::tableboxcontrol::Base*>( this->base )->tableContainer.selectRow( row );
}

void
TableBox::setOffsets( int x, int y )
{
	dynamic_cast<openocl::controls::tableboxcontrol::Base*>( this->base )->scrollPane->setOffsets( x, y );
}

int
TableBox::getOffsetOfRow( int row ) const
{
	int offset = -1;
	if ( 0 <= row )
	{
		offset = dynamic_cast<openocl::controls::tableboxcontrol::Base*>( this->base )->tableCanvas.getOffsetOfRow( row );
	}
	return offset;
}
