/*
 *  Copyright (c) 2005 Daniel Robert Bradley.    All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_APPLICATION_APPLICATION_H
#define OPENOCL_APPLICATION_APPLICATION_H

#include "openocl/application.h"
#include <openocl/base.h>
#include <openocl/base/NoSuchElementException.h>
#include <openocl/base/Object.h>
#include <openocl/base/event/EventListener.h>
#include <openocl/ui.h>
#include <openocl/util.h>

namespace openocl {
	namespace application {

class Application : public openocl::base::Object,
                    public openocl::base::event::EventListener
{
public:
	static const mask SDI    = 1;
	static const mask MDI    = 2;
	static const mask TABBED = 3;
	static const mask FDI    = 4;

	virtual ~Application();
	void init_stage1();
	void init_stage2();

	void forceRemoveDocuments();
	void finit();

	//	public virtual methods (EventListener)
	virtual void deliverEvent( const openocl::base::event::Event& anEvent );

	//	public abstract virtual methods (Application)
	virtual void deliverActionEvent( const openocl::base::event::ActionEvent& anEvent ) = 0;

	//	public abstract virtual methods
	virtual void newDocument( const openocl::base::String& data ) = 0;
	virtual void openDocument() = 0;
	virtual void saveDocument();
	virtual void saveDocumentAs();
	virtual bool closeDocument();
	virtual bool closeDocument( const openocl::base::String& documentName );
	virtual bool closeAllDocuments(); 
	virtual bool closeApplication(); 

	//virtual void update() = 0;

	//	public methods
	bool loadConfiguration();
	bool loadPreferences();
	void start();
	void stop();

	void      addDocument( Document* aDocument );
	Document* removeDocument( const openocl::base::String& documentId ) throw (openocl::base::NoSuchElementException*);
	Document* removeActiveDocument() throw (openocl::base::NoSuchElementException*);

	void addApplicationMenuItem( const openocl::base::String& menuItemId, openocl::ui::MenuItem& aMenu );
	void removeApplicationMenuItem( openocl::ui::MenuItem& aMenu );
	void addToolBar( const openocl::base::String& toolbarId, openocl::ui::ToolBar& aToolBar );
	void removeToolBar( const openocl::base::String& toolbarId );

	void documentToFront( const openocl::base::String& documentId );

	void setApplicationTitle( const openocl::base::String& applicationTitle );
	void setApplicationLogo( const openocl::base::String& anImageId );
	//void setCurrentDocumentId( const openocl::base::String& aDocumentId );

	//	public methods
	openocl::ui::Visual& getVisual();
	openocl::ui::ApplicationFrame& getApplicationFrame();
	Document& getActiveDocument() throw (openocl::base::NoSuchElementException*);

	//	public constant methods
	const openocl::base::String& getApplicationTitle() const;

	unsigned int getNrOfDocuments() const;
	bool hasOpen( const openocl::base::String& aDocumentName ) const;
	//const openocl::base::String& getCurrentDocumentId() const;

	const openocl::util::Properties& getConfiguration() const;
	const openocl::util::Properties& getPreferences() const;

protected:
	Application( flags type = TABBED );
	Application( DocumentManager* aDocumentManager );

private:
	openocl::ui::Visual*           visual;
	openocl::ui::ApplicationFrame* applicationFrame;
	openocl::application::DocumentManager* documentManager;
	openocl::base::String*         applicationTitle;
	bool                           run;

	openocl::util::Index* menuItems;
	openocl::util::Index* toolbars;
	openocl::util::Dictionary* documents;

	openocl::ui::RuntimeSummaryItem* rsi;
	openocl::ui::Menu*         masterMenu;
	openocl::ui::MenuGroup*    menuGroup;
	openocl::ui::ToolBarGroup* toolBarGroup;
	openocl::ui::TabbedView*   tabbedView;

	openocl::util::Properties* configuration;
	openocl::util::Properties* preferences;

	static unsigned int nextDocumentId;
};

};};

#endif
