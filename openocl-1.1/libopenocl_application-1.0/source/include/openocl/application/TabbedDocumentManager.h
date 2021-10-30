/*
 *  Copyright (c) 2005 Daniel Robert Bradley.    All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_APPLICATION_TABBEDDOCUMENTMANAGER_H
#define OPENOCL_APPLICATION_TABBEDDOCUMENTMANAGER_H

#include "openocl/application.h"
#include "openocl/application/DocumentManager.h"
#include <openocl/ui.h>
#include <openocl/util.h>
#include <openocl/base/Object.h>

namespace openocl {
	namespace application {

class TabbedDocumentManager : public openocl::base::Object, public DocumentManager
{
public:
	TabbedDocumentManager( openocl::ui::Visual& aVisual, Application& parent );
	~TabbedDocumentManager();

	//	public abstract virtual methods (DocumentManager)
	virtual void addDocument( Document* aDocument );
	virtual Document* removeDocument( const openocl::base::String& documentId ) throw (openocl::base::NoSuchElementException*);
	virtual Document* removeActiveDocument() throw (openocl::base::NoSuchElementException*);
	virtual openocl::ui::Component& getDocumentManagerComponent() throw (openocl::base::NoSuchElementException*);
	virtual Document& getActiveDocument() throw (openocl::base::NoSuchElementException*);
	
	//	public abstract virtual constant methods
	virtual openocl::util::Sequence& getDocumentComponents() const;

	virtual unsigned int getNrOfDocuments() const;
	virtual bool hasOpen( const openocl::base::String& aDocumentKey ) const;

private:
private:
	openocl::ui::Visual& visual;
	openocl::application::Application& parent;
	
	openocl::util::Dictionary* documents;
	openocl::ui::TabbedView* tabbedView;
	openocl::util::Sequence* components;
};

};};

#endif
