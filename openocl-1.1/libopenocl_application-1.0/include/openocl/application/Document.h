/*
 *  Copyright (c) 2005 Daniel Robert Bradley.    All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_APPLICATION_DOCUMENT_H
#define OPENOCL_APPLICATION_DOCUMENT_H

#include "openocl/application.h"

#include <openocl/base.h>
#include <openocl/base/NoSuchElementException.h>
#include <openocl/base/event.h>
#include <openocl/mvc.h>
#include <openocl/ui.h>
#include <openocl/util.h>
#include <openocl/threads/Thread.h>

#include <openocl/base/event/EventListener.h>

namespace openocl {
	namespace application {

class Document : public openocl::threads::Thread, public openocl::base::event::EventListener
{
friend class Application;
public:
	virtual ~Document();

	//	public virtual methods (EventListener)
	virtual void deliverEvent( const openocl::base::event::Event& anEvent );

	//	public pure virtual methods (Document) Must be implemented by subclasses
	virtual void init( const openocl::base::Values& values ) = 0;

	//	public virtual methods (Document) To be overriden by subclasses
	virtual bool handleActionEvent( const openocl::base::event::ActionEvent& anEvent );

	virtual void saveDocument( int typeId );
	virtual void exportDocument( const openocl::base::String& location, int typeId );

	//	public virtual methods
	virtual void forwardEvent( const openocl::base::event::Event& event );
	virtual void saveDocument();
	
	//	public methods
	void addAuxiliaryView( openocl::mvc::View& anAuxView );
	void setApplication( Application& anApplication );
	void setChanged( bool changed );
	void setDocumentAlias( const openocl::base::String& documentAlias );
	void setDocumentKey( const openocl::base::String& documentKey );
	void setDocumentLocation( const openocl::base::String& documentName );
	void setProvisionalDocumentLocation( const openocl::base::String& documentName );
	void setView( openocl::mvc::View& aView );
	void setVisual( openocl::ui::Visual& aVisual );
	void stop();

	openocl::ui::Component& getViewComponent();
	openocl::threads::EventQueue& getEventQueue();
	openocl::ui::Visual& getVisual();

	openocl::util::Sequence& getToolbars();

	//	public methods
	openocl::application::Application& getApplication();

	//	public virtual constant methods
	virtual const openocl::base::String& getDisplayName() const;
	virtual const openocl::base::String& getDocumentAlias() const;
	virtual const openocl::base::String& getDocumentKey() const;
	virtual const openocl::base::String& getDocumentLocation() const;
	virtual const openocl::base::String& getProvisionalDocumentLocation() const;
	virtual bool hasChanged() const;
	virtual bool hasName() const;

	//	public constant methods
	const openocl::ui::Component& getViewComponent() const;

protected:
	//	protected constructor
	Document();

	//	protected virtual methods (Thread)
	virtual bool run();

	//	protected virtual methods (Document)
	virtual void runSaveDocument( int typeId );
	virtual void runExportDocument( const openocl::base::String& location, int typeId );

private:
	//	private methods
	void handleCloseEvent( const openocl::base::event::ActionEvent& ae );

private:
	openocl::ui::Component*       defaultViewComponent;
	openocl::threads::EventQueue* eventQueue;
	
	openocl::base::String*        documentKey;
	openocl::base::String*        documentAlias;
	openocl::base::String*        documentLocation;
	openocl::base::String*        provisionalDocumentLocation;
	
	openocl::mvc::View*      view;      //  Main document view
	openocl::util::Sequence* toolbars;  //  Document related toolbars
	openocl::util::Sequence* auxViews;  //  Auxiliary floating frames

	openocl::ui::Visual* visual;
	openocl::application::Application* application;

	bool changed;
};

};};

#endif
