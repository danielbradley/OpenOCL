//#include <imb/jsh/visualization/icons/icon_new.xpm>
//#include <imb/jsh/visualization/icons/icon_open.xpm>
//#include <imb/jsh/visualization/icons/icon_save.xpm>

#include <icons/acb50.xpm>

#include <openocl/application.h>
#include <openocl/application/Application.h>
#include <openocl/base.h>
#include <openocl/base/String.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/controls/FileChooser.h>
#include <openocl/controls/WarningBox.h>
#include <openocl/imaging/Image.h>
#include <openocl/imaging/formats/XPixmap.h>
#include <openocl/ui/ApplicationFrame.h>
#include <openocl/ui/Dialog.h>
#include <openocl/ui/Menu.h>
#include <openocl/ui/OffscreenImageManager.h>
#include <openocl/ui/ToolBar.h>
#include <openocl/ui/Visual.h>
#include <openocl/controls/FileChooser.h>

using namespace openocl::application;
using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::controls;
using namespace openocl::imaging;
using namespace openocl::imaging::formats;
using namespace openocl::ui;

class Mini : public openocl::application::Application
{
public:
	Mini()
	{
		OffscreenImageManager& oim = this->getVisual().getOffscreenImageManager();

		Image* image1 = new XPixmap( (const char**) acb50 );
//		Image* image2 = new XPixmap( (const char**) icon_open );
//		Image* image3 = new XPixmap( (const char**) icon_save );

		oim.addImage( "ACB",  image1 );
//		oim.addImage( "OPEN", image2 );
//		oim.addImage( "SAVE", image3 );
	
//		this->appToolBar = new ToolBar();
//		this->appToolBar->addIcon( String( "new" ),  String( "NEW" ), String( "New" ), 75, 0 );
//		this->appToolBar->addIcon( "new",  "NEW", "New", "New", 75, 0 );
//		this->appToolBar->addIcon( "open", "OPEN", "Open", "OPEN", 75, 0 );
//		this->appToolBar->addIcon( "save", "SAVE", "Save", 75, 0 );
//		this->addToolBar( "APPLICATION", *this->appToolBar );

		this->menuFile = new Menu( "File", "FILE", "File" );
		{
			this->menuFile->addMenuItem( new MenuItem( "Open MenuItem", "OPEN", "Open" ) );
		}
		this->addApplicationMenuItem( "file", *this->menuFile );
	}
	
	~Mini()
	{
		//	Delete menus
		this->removeApplicationMenuItem( *this->menuFile );
		delete this->menuFile;

		//delete this->appToolBar;
	}

	virtual void newDocument( const String& name )
	{}

	virtual void openDocument()
	{
		Panel& panel = this->getApplicationFrame().getTopPanel();
		WarningBox warning_box( "Are you sure you want to open a file", "Confirm", "Cancel" );
		FileChooser chooser;
		Dialog::showDialog( panel, warning_box );
		Dialog::showDialog( panel, chooser );
	}

	void deliverActionEvent( const ActionEvent& ae )
	{}

	Menu* menuFile;	
	ToolBar* appToolBar;
};


int main( int argc, char** arv )
{
	Mini me;
	me.setApplicationTitle( "Mini me" );
	me.setApplicationLogo( "ACB" );
	me.start();
	return 0;
}
