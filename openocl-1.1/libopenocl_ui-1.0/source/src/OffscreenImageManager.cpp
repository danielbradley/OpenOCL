/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include <icons/bluecurve/32x32/stock/gtk-open.xpm>
#include <icons/kdeclassic/32x32/mimetypes/mime_empty.xpm>
#include <icons/slick/48x48/desktop.xpm>
#include <icons/islabs/48x48/earth.xpm>
#include <icons/slick/48x48/folder_home.xpm>
//#include <icons/islabs/48x48/folder_tux_2.xpm>
#include <icons/slick/48x48/hdd_unmount.xpm>

#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/Panel.h"
#include "openocl/ui/OffscreenImage.h"
#include "openocl/ui/OffscreenImageManager.h"
#include <openocl/base/Iterator.h>
#include <openocl/base/Runtime.h>
#include <openocl/base/String.h>
#include <openocl/imaging/Image.h>
#include <openocl/imaging/formats/XPixmap.h>
#include <openocl/util/Dictionary.h>
#include <openocl/util/StringKey.h>

#include <cstdio>

using namespace openocl::base;
using namespace openocl::imaging;
using namespace openocl::imaging::formats;
using namespace openocl::ui;
using namespace openocl::util;

OffscreenImageManager::OffscreenImageManager()
{
	this->images = new Dictionary();
	this->cache  = new Dictionary();
}

OffscreenImageManager::~OffscreenImageManager()
{
	delete this->images;
	delete this->cache;
}

void
OffscreenImageManager::loadDefaultImages( const GraphicsContext& gc )
{
	OffscreenImage* oi = null;

	oi = gc.createOffscreenImage( XPixmap( (const char**) gtk ) );
	this->cache->put( new StringKey( "FOLDER" ), oi );

	oi = gc.createOffscreenImage( XPixmap( (const char**) desktop_icon ) );
	this->cache->put( new StringKey( "DESKTOP" ), oi );

	oi = gc.createOffscreenImage( XPixmap( (const char**) folder_home ) );
	this->cache->put( new StringKey( "HOME" ), oi );

	oi = gc.createOffscreenImage( XPixmap( (const char**) hdd_unmount ) );
	this->cache->put( new StringKey( "LOCAL" ), oi );
			
	oi = gc.createOffscreenImage( XPixmap( (const char**) earth_xpm ) );
	this->cache->put( new StringKey( "NETWORK" ), oi );

	oi = gc.createOffscreenImage( XPixmap( (const char**) earth_xpm ) );
	this->cache->put( new StringKey( "SYSTEM" ), oi );

	oi = gc.createOffscreenImage( XPixmap( (const char**) mime_empty ) );
	this->cache->put( new StringKey( "UNKNOWN" ), oi );
}

void
OffscreenImageManager::addImage( const String& anImageId, Image* anImage )
{
	this->images->put( new StringKey( anImageId ), anImage );
}

void
OffscreenImageManager::cacheImages( const GraphicsContext& gc )
{
	Iterator* it = this->images->keys();
	while ( it->hasNext() )
	{
		const Key& key = dynamic_cast<const Key&>( it->next() );
		const String* skey = key.toString();

		try
		{
			this->cache->getValue( StringKey( *skey ) );
		} catch ( NoSuchElementException* ex ) {
			delete ex;
			const Image& image = dynamic_cast<const Image&>( this->images->getValue( StringKey( *skey ) ) );
			OffscreenImage* oi = gc.createOffscreenImage( image );
			this->cache->put( new StringKey( *skey ), oi );
		}
		delete skey;
	}
	delete it;
}

const OffscreenImage&
OffscreenImageManager::findImage( const GraphicsContext& gc, const String& imageId )
throw ( NoSuchElementException* )
{
	const OffscreenImage* osi;

	StringKey key( imageId );
	try
	{
		osi = dynamic_cast<const OffscreenImage*>( &this->cache->getValue( key ) );
	} catch ( NoSuchElementException* ex ) {
		delete ex;
		osi = null;
	}
	
	if ( !osi )
	{	
		OffscreenImage* oi = null;
		
		if ( 0 == imageId.compareTo( "FOLDER" ) )
		{
			oi = gc.createOffscreenImage( XPixmap( (const char**) gtk ) );
		}
		else if ( 0 == imageId.compareTo( "DESKTOP" ) )
		{
			oi = gc.createOffscreenImage( XPixmap( (const char**) desktop_icon ) );
		}
		else if ( 0 == imageId.compareTo( "HOME" ) )
		{
			oi = gc.createOffscreenImage( XPixmap( (const char**) folder_home ) );
		}
		else if ( 0 == imageId.compareTo( "LOCAL" ) )
		{
			oi = gc.createOffscreenImage( XPixmap( (const char**) hdd_unmount ) );
		}
		else if ( 0 == imageId.compareTo( "NETWORK" ) )
		{
			oi = gc.createOffscreenImage( XPixmap( (const char**) earth_xpm ) );
		}
		else if ( 0 == imageId.compareTo( "SYSTEM" ) )
		{
			oi = gc.createOffscreenImage( XPixmap( (const char**) earth_xpm ) );
		}
		else if ( 0 == imageId.compareTo( "UNKNOWN" ) )
		{
			oi = gc.createOffscreenImage( XPixmap( (const char**) mime_empty ) );
		}
		else
		{
			const Image& image = dynamic_cast<const Image&>( this->images->getValue( key ) );
			oi = gc.createOffscreenImage( image );
		}
		this->cache->put( new StringKey( imageId ), oi );
		osi = oi;
	}
	return *osi;
}
