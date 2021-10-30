/*
 *  Copyright (C) 1997-2004 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include <openocl/mvc/DocumentPaneMonitor.h>

#include <openocl/base/NoSuchElementException.h>
#include <openocl/threads/Mutex.h>
#include <openocl/ui/Component.h>
#include <openocl/ui/Panel.h>
#include <openocl/util/Sequence.h>

#include <cstdio>

using namespace openocl::base;
using namespace openocl::mvc;
using namespace openocl::threads;
using namespace openocl::ui;
using namespace openocl::util;

DocumentPaneMonitor::DocumentPaneMonitor()
{
	this->nextMutex = new Mutex();
	this->next = null;
	this->refreshed = false;
}

DocumentPaneMonitor::~DocumentPaneMonitor()
{
	delete this->nextMutex;
}

void
DocumentPaneMonitor::doLayout( const GraphicsContext& gc )
{
	fprintf( stderr, "DocumentPaneMonitor::doLayout()\n" );

	fprintf( stderr, "DocumentPaneMonitor::doLayout(): waiting for lock\n" );
	this->nextMutex->lock( "DocumentPaneMonitor::doLayout" );
	fprintf( stderr, "DocumentPaneMonitor::doLayout(): got lock\n" );
	
	if ( next )
	{
		fprintf( stderr, "DocumentPaneMonitor::doLayout(): acquiring next\n" );
		Sequence& components = this->getComponents();
		if ( components.getLength() )
		{
			Component& com = dynamic_cast<Component&>( components.elementAt( 0 ) );
			this->remove( com );
		}
		this->add( *this->next );
		this->next = null;
	}
	
	if ( !this->refreshed )
	{
		try
		{
			this->getContainingPanel().relayout();
			this->refreshed = true;
		} catch ( NoSuchElementException* ex ) {
			delete ex;
		}
	}

	this->nextMutex->unlock( "DocumentPaneMonitor::doLayout" );
	fprintf( stderr, "DocumentPaneMonitor::doLayout(): released lock\n" );

	this->DocumentPane::doLayout( gc );
}

void
DocumentPaneMonitor::setNextComponent( Component& com )
{
	fprintf( stderr, "DocumentPaneMonitor::setNextComponent(): waiting for lock\n" );
	this->nextMutex->lock( "DocumentPaneMonitor::setNextComponent" );
	{
		fprintf( stderr, "DocumentPaneMonitor::setNextComponent(): got lock\n" );

		fprintf( stderr, "DocumentPaneMonitor::setNextComponent(): setting next\n" );
		this->next = &com;
		this->refreshed = false;
	}
	this->nextMutex->unlock( "DocumentPaneMonitor::setNextComponent" );

	try
	{
		this->getContainingPanel().relayout();//refresh();
	} catch ( NoSuchElementException* ex ) {
		delete ex;
		fprintf( stderr, "DocumentPaneMonitor::setNextComponent: could not get panel for relayout!\n" );
	}

	fprintf( stderr, "DocumentPaneMonitor::setNextComponent(): released lock\n" );
}

void
DocumentPaneMonitor::releaseComponent( Component& com )
{
	fprintf( stderr, "DocumentPaneMonitor::releaseComponent: waiting for lock\n" );
	this->nextMutex->lock( "DocumentPaneMonitor::releaseComponent" );
	{
		fprintf( stderr, "DocumentPaneMonitor::releaseComponent: got lock\n" );
		fprintf( stderr, "DocumentPaneMonitor::releaseComponent: removing com\n" );
		Sequence& components = this->getComponents();
		components.remove( com );
		this->refreshed = false;
	}
	this->nextMutex->unlock( "DocumentPaneMonitor::releaseComponent" );
	fprintf( stderr, "DocumentPaneMonitor::releaseComponent: released lock\n" );
}

