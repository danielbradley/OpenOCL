#include <openocl/base/Exception.h>
#include <openocl/base/String.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/EventListener.h>
#include <openocl/ui/ApplicationFrame.h>
#include <openocl/ui/Button.h>
#include <openocl/ui/ButtonGroup.h>
#include <openocl/ui/ComboBox.h>
#include <openocl/ui/Dialog.h>
#include <openocl/ui/Panel.h>
#include <openocl/ui/ToolBar.h>
#include <openocl/ui/Visual.h>
#include <openocl/ui/WindowFrame.h>

#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::ui;

class Listener : public Object, public EventListener
{
public:
	Listener( Panel& aPanel ) : parent( aPanel )
	{}

void deliverEvent( const Event& anEvent )
{
	switch ( anEvent.getType() )
	{
	case Event::ACTION_EVENT:
		{
			fprintf( stdout, "Action event\n" );
		
			Button* close_button = new Button( "Close", "CLOSE_FRAME.", "Close" );
			Dialog::showDialog( parent, *close_button );

			delete close_button;
		}
		break;
	}
}

Panel& parent;

};


int main( int argc, char** argv )
{
	try
	{
		bool loop  = true;

		//	First it is necessary to retrive a visual instance.
		//
		Visual* visual = Visual::getInstance();
		ApplicationFrame* frame = new ApplicationFrame( *visual, &loop );

		Listener* listener = new Listener( frame->getTopPanel() );

		frame->setName( "Buttons Example" );

		Button* one   = new Button( "Show dialog", "SHOW", "Show Dialog" );

		ButtonGroup* button_group_one = new ButtonGroup( LayoutManager::EASTWARD | LayoutManager::LEFT | LayoutManager::MIDDLE );
		button_group_one->add( one );
		button_group_one->addEventListener( *listener );

		Panel& top_panel = frame->getTopPanel();
		top_panel.add( button_group_one );

		frame->positionAndResize( 900, 240 );
		frame->setVisible( true );
		
		visual->enterEventLoop( &loop );
	} catch ( Exception* ex ) {
		delete ex;
		return -1;
	}
	return 0;
}
