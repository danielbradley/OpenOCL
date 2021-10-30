/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/Dimensions.h"
#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/Panel.h"
#include "openocl/ui/ProgressIndicator.h"

#include <openocl/base/FormattedString.h>
#include <openocl/base/Math.h>
#include <openocl/base/String.h>
#include <openocl/base/event/Event.h>
#include <openocl/imaging/Color.h>
#include <openocl/imaging/TextExtents.h>

#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::ui;


const double ProgressIndicator::initialProgress = 0.0;

ProgressIndicator::ProgressIndicator( flags typeAndStyle )
: Component( "Progress Indicator" ), typeAndStyle( typeAndStyle ), thickness( 10 )
{
	this->setEdge( 5 );
	this->setMargin( 0 );
	this->setBorder( 0 );
	this->setPadding( 5 );

	this->progress = initialProgress;
	this->progressLength = (unsigned int) initialProgress;

	//this->progressString = new FormattedString( "%3.0f (%%)", initialProgress * 100 );

}

ProgressIndicator::~ProgressIndicator()
{
	//delete this->progressString;
}

void
ProgressIndicator::processEvent( const Event& anEvent )
{}

void
ProgressIndicator::doLayout( const GraphicsContext& gc )
{}

void
ProgressIndicator::setProgress( double progress )
{
//	fprintf( stderr, "ProgressIndicator::setProgress( %f )\n", progress );
	if ( this->progress != progress )
	{
		this->progress = progress;

		//delete this->progressString;
		//this->progressString = new FormattedString( "%3.0f (%%)", progress * 100.0 );
		this->fireChangeEvent();
		if ( this->progress >= 1.0 )
		{
			this->getContainingPanel().relayout();
		} else {
			this->getContainingPanel().redraw();
		}
	}
}

Dimensions
ProgressIndicator::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	Dimensions d;

	if ( this->progress < 1.0 )
	{
		unsigned int pad = this->getContentInset()*2;
	
		switch ( this->typeAndStyle )
		{
		case VERTICAL_BAR:
			d.width = pad + this->thickness;
			d.height = height;
			break;
		case DIGITAL:
			{
				TextExtents* extents = gc.measureTextExtents( *this->progressString );
				d.width  = pad + extents->width;
				d.height = pad + extents->ascent + extents->descent;
				delete extents;
			}
			break;
		case HORIZONTAL_BAR:
		default:
			d.width = width;
			d.height = pad + this->thickness;
		}
	}
	return d;
}

bool
ProgressIndicator::draw( GraphicsContext& gc ) const
{
	bool changed = this->hasChanged();
	
	if ( changed )
	{
		Region bounds   = this->getBounds();
		Region content  = this->getContentRegion();
		Region progress = content;

		gc.setFill( gc.getBackground() );
		gc.drawFilledRectangleRegion( bounds );
		
		gc.setFill( Color::WHITE );
		gc.drawFilledRectangleRegion( content );

		progress.width = (unsigned int) (progress.width * this->progress);
		progress.x2 = progress.x1 + progress.width - 1;

		gc.setFill( Color::BLUE );
		gc.drawFilledRectangleRegion( progress );

/*
		if ( progress < 1.0 )
		{
			Region box = this->getBox();
			Region progress_bar = *this->progressBar;
		
			switch ( this->typeAndStyle )
			{
			case DIGITAL:
				gc.setForeground( Color::BLACK );
				gc.drawString( *this->progressString, box.x1, box.y2 );
				break;
			case HORIZONTAL_BAR:
			default:
				gc.setFill( Color::CANVAS );
				gc.drawFilledRectangleRegion( box );
				gc.setFill( Color::BLUE );
				gc.drawFilledRectangle( progress_bar.x1, progress_bar.y1, this->progressLength, progress_bar.height );
				this->drawInward3DBox( gc, box, Color::SHINE2, Color::SHADE1 );
			}
		}
*/
	}
	this->setChanged( false );
	return false;
}
