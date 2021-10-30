#include "openocl/textviewer/TextViewerControl.h"
#include "openocl/textviewer/TextViewerModel.h"

#include <openocl/base/String.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/controls/FileBox.h>
#include <openocl/controls/LocationBar.h>
#include <openocl/controls/ShortcutBox.h>
#include <openocl/imaging/Color.h>
#include <openocl/io/Path.h>
#include <openocl/ui/Container.h>
#include <openocl/ui/OpaqueContainer.h>
#include <openocl/ui/DirectedLayout.h>
#include <openocl/ui/FocusManager.h>
#include <openocl/ui/Label.h>
#include <openocl/ui/LayoutManager.h>
#include <openocl/ui/Panel.h>
#include <openocl/ui/ProgressIndicator.h>
#include <openocl/ui/ScrollPane.h>
#include <openocl/ui/TextField.h>
#include <openocl/ui/TextCanvas.h>

#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::controls;
using namespace openocl::imaging;
using namespace openocl::io;
using namespace openocl::ui;
using namespace openocl::textviewer;

namespace doc {
	namespace demo {
		namespace democontrol {

class BaseContainer : public openocl::ui::Container
{
public:
	BaseContainer( TextViewerModel& aModel, FocusManager& focusManager )
	: Container(),
	  model( aModel ),
	  progress( ProgressIndicator::HORIZONTAL_BAR )
	{
		#ifdef TRIAL
		this->setLayout( new DirectedLayout( LayoutManager::NORTHWARD ) );
		this->scrollPane = new ScrollPane( this->text );
		this->panel = this->getContainingPanel().createSubPanel();
		this->panel->show( Panel::SHOW );
		this->panel->relayout();

			//this->popup->requestGeometry( x, y, width, height, 0 );

			//this->popup->toFront();
			//this->popup->show( Panel::HIDDEN );
			//this->popup->show( Panel::SHOW );
			//this->popup->getTopPanel().relayout();


		this->panel->add( *this->scrollPane );
		this->add( *this->panel );
		this->add( progress );


		#else
		this->setLayout( new DirectedLayout( LayoutManager::NORTHWARD ) );
		this->scrollPane = new ScrollPane( this->text );
		this->add( *this->scrollPane );
		this->add( progress );
		#endif
	}
	
	~BaseContainer()
	{
		this->remove( *this->scrollPane );
		this->remove( progress );
		
		delete this->scrollPane;
	}

	void refresh( const String& id )
	{
		//fprintf( stderr, "TextViewerControl::refresh: %s\n", id.getChars() );
		if ( id.startsWith( "TextViewerModel.data" ) )
		{
			String* data = this->model.getData();
			this->text.setText( *data );
			delete data;
		}
		else if ( id.startsWith( "Progress" ) )
		{
			progress.setProgress( this->model.getProgress() );
		}
	}

	Panel* panel;
	ScrollPane* scrollPane;
	TextViewerModel& model;
	ProgressIndicator progress;
	TextCanvas text;
};

};};};


TextViewerControl::TextViewerControl( TextViewerModel& aModel ) : Control( "doc::demo::TextViewerControl" )
{
	this->base = new doc::demo::democontrol::BaseContainer( aModel, this->getFocusManager() );
	this->base->addEventListener( *this );
	this->setControlComponent( *this->base );
}

TextViewerControl::~TextViewerControl()
{
	this->getFocusManager().purge();
	this->base->removeEventListener( *this );
	delete this->base;
}

bool
TextViewerControl::handleActionEvent( const ActionEvent& anActionEvent )
{
	bool handled = false;
	return handled;
}

void
TextViewerControl::refresh( const String& id )
{
	dynamic_cast<doc::demo::democontrol::BaseContainer*>( this->base )->refresh( id );
}
