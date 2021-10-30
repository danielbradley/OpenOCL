/*
 *  Copyright (c) 2005 Daniel Robert Bradley.    All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/application.h"
#include "openocl/application/Document.h"
#include "openocl/application/TabbedDocumentManager.h"
#include <openocl/base/Iterator.h>
#include <openocl/base/Runtime.h>
#include <openocl/base/String.h>
#include <openocl/base/StringBuffer.h>
#include <openocl/ui.h>
#include <openocl/ui/Component.h>
#include <openocl/ui/TabbedView.h>
#include <openocl/ui/Panel.h>
#include <openocl/util/Dictionary.h>
#include <openocl/util/Key.h>
#include <openocl/util/Sequence.h>
#include <openocl/util/StringKey.h>

#include <cstdlib>
#include <cstdio>

using namespace openocl::application;
using namespace openocl::base;
using namespace openocl::ui;
using namespace openocl::util;

TabbedDocumentManager::TabbedDocumentManager( Visual& visual, Application& parent )
: Object(), visual( visual ), parent( parent )
{
	this->documents  = new Dictionary();
	this->tabbedView = new TabbedView();
	this->components = new Sequence();
}

TabbedDocumentManager::~TabbedDocumentManager()
{
	delete this->components;
	delete this->tabbedView;
	delete this->documents;
}

void
TabbedDocumentManager::addDocument( Document* aDocument )
{
	//	Documents should have a valid unique name before being
	//	added to a document manager.

	aDocument->setVisual( this->visual );
	aDocument->setApplication( this->parent );
	
	const String& index = aDocument->getDocumentKey();
	StringKey key( index );
	if ( index.equals( "" ) || this->documents->containsKey( key ) )
	{
		fprintf( stderr, "TabbedDocumentManager: Document::getDocumentKey() method did not return unique key, aborting!\n" );
		abort();
	}
	
	this->documents->put( new StringKey( index ), aDocument );
	
	Component& view_component = aDocument->getViewComponent();
	this->tabbedView->add( view_component );
	this->tabbedView->getContainingPanel().relayout();//update();

	aDocument->start();
}

Document*
TabbedDocumentManager::removeDocument( const String& documentId )
throw (NoSuchElementException*)
{
	StringKey key( documentId );
	Object* obj = this->documents->removeValue( key );
	Document* doc = dynamic_cast<Document*>( obj );
	this->tabbedView->getContainingPanel().relayout();//update();
	
	return doc;
}

Document*
TabbedDocumentManager::removeActiveDocument()
throw (NoSuchElementException*)
{
	Document* document = null;
	Component& com = this->tabbedView->getActiveComponent();
	this->tabbedView->remove( com );
	this->tabbedView->getContainingPanel().relayout();//update();

	Iterator* keys = this->documents->keys();
	while ( keys->hasNext() )
	{
		const Key& key = dynamic_cast<const Key&>( keys->next() );
		const Object& obj = this->documents->getValue( key );
		const Document& doc = dynamic_cast<const Document&>( obj );
		const Component& view_com = doc.getViewComponent();
			
		if ( &com == &view_com )
		{
			Object* obj = this->documents->removeValue( key );
			document = dynamic_cast<Document*>( obj );
			break;
		}
	}
	delete keys;

	if ( null == document )
	{
		throw new NoSuchElementException();
	}
	
	return document;
}

Component&
TabbedDocumentManager::getDocumentManagerComponent()
throw (NoSuchElementException*)
{
	return *this->tabbedView;
}

Document&
TabbedDocumentManager::getActiveDocument()
throw (NoSuchElementException*)
{
	
	Document* document = null;
	Component& com = this->tabbedView->getActiveComponent();

	Iterator* keys = this->documents->keys();
	while ( keys->hasNext() )
	{
		const Key& key = dynamic_cast<const Key&>( keys->next() );
		const Object& obj = this->documents->getValue( key );
		const Document& doc = dynamic_cast<const Document&>( obj );
		const Component& view_com = doc.getViewComponent();
			
		if ( &com == &view_com )
		{
			Object& obj = this->documents->getValue( key );
			document = &dynamic_cast<Document&>( obj );
			break;
		}
	}
	delete keys;

	if ( null == document )
	{
		throw new NoSuchElementException();
	}
	
	return *document;
}

Sequence&
TabbedDocumentManager::getDocumentComponents() const
{
	return *this->components;
}

unsigned int
TabbedDocumentManager::getNrOfDocuments() const
{
	return this->documents->size();
}

bool
TabbedDocumentManager::hasOpen( const String& documentKey ) const
{
	StringKey key( documentKey );
	return this->documents->containsKey( key );
}
