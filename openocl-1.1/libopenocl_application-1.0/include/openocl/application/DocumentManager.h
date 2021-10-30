/*
 *  Copyright (c) 2005 Daniel Robert Bradley.    All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_APPLICATION_DOCUMENTMANAGER_H
#define OPENOCL_APPLICATION_DOCUMENTMANAGER_H

#include "openocl/application.h"
#include <openocl/base/Interface.h>
#include <openocl/base/NoSuchElementException.h>

namespace openocl {
	namespace application {

class DocumentManager : public openocl::base::Interface
{
public:
	//	public abstract virtual methods
	virtual void addDocument( Document* aDocument ) = 0;
	virtual Document* removeDocument( const openocl::base::String& documentId ) throw (openocl::base::NoSuchElementException*) = 0;
	virtual Document* removeActiveDocument() throw (openocl::base::NoSuchElementException*) = 0;
	virtual openocl::ui::Component& getDocumentManagerComponent() throw (openocl::base::NoSuchElementException*) = 0;
	virtual Document& getActiveDocument() throw (openocl::base::NoSuchElementException*) = 0;
	
	//	public abstract virtual constant methods

	virtual unsigned int getNrOfDocuments() const = 0;
	virtual bool hasOpen( const openocl::base::String& aDocumentName ) const = 0;

};

};};

#endif
