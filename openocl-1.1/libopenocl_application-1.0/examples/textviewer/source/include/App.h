#ifndef APP_H
#define APP_H

#include <openocl/application/Application.h>
#include <openocl/base.h>
#include <openocl/ui.h>
#include <openocl/util.h>

class App : public openocl::application::Application
{
public:
	App( int argc, char** argv );
	~App();

	virtual void deliverActionEvent( const openocl::base::event::ActionEvent& anActionEvent );

	//	public virtual methods (Application)
	virtual void newDocument( const openocl::base::String& name );
	virtual void openDocument();
	virtual void saveDocument();
	virtual void saveDocumentAs();
	virtual bool closeDocument();
	virtual bool closeDocument( const openocl::base::String& documentName );
	virtual bool closeAllDocuments();
	virtual bool closeApplication();

private:

	//	private initialization methods
	void parseArguments();

	//	private methods
	void openDocument( const openocl::base::String& location );

	//	private members
	openocl::util::Properties* arguments;
	openocl::ui::Menu* menuFile;
	openocl::ui::Menu* menuDoc;
};

#endif
