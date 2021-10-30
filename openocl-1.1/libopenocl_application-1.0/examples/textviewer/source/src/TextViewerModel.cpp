#include "openocl/textviewer/TextViewerModel.h"
#include <openocl/base/FormattedString.h>
#include <openocl/base/String.h>
#include <openocl/base/StringBuffer.h>
#include <openocl/base/event/Event.h>
#include <openocl/io/File.h>
#include <openocl/io/FileInputStream.h>
#include <openocl/io/LineParser.h>
#include <openocl/io/Path.h>
#include <openocl/util/Sequence.h>

#include <cstdlib>
#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::io;
using namespace openocl::util;
using namespace openocl::textviewer;

//----------------------------------------------------------------
//	constructors
//----------------------------------------------------------------

TextViewerModel::TextViewerModel()
{
	this->lines = null;
	this->setProgress( 0.0 );
}

TextViewerModel::~TextViewerModel()
{
	delete this->lines;
}

//----------------------------------------------------------------
//	public virtual methods (EventListener)
//----------------------------------------------------------------

void
TextViewerModel::deliverEvent( const Event& anEvent )
{}

//----------------------------------------------------------------
//	public async methods
//----------------------------------------------------------------

void
TextViewerModel::load( const String& location )
{
	//fprintf( stderr, "TextViewerModel::load( %s )\n", location.getChars() );

	try
	{
		this->lock( "TextViewerModel::initialize()" );
		{
			this->lines = new Sequence();
		}
		this->unlock( "TextViewerModel::initialize()" );

		double progress = 0.0;
		this->setProgress( progress );

#ifdef DEBUG_MODEL
		this->lines->add( new String( "Debugging model!" ) );

		for ( unsigned int i=0; i < 90; i++ )
		{
			this->lines->add( new FormattedString( "progress: %f", progress ) );
			this->setProgress( (progress += 0.01) );
		}

#elif DEBUG_PROGRESS
		this->lines->add( new String( "Debugging progress!" ) );

		for ( unsigned int i=0; i < 10000; i++ )
		{
			this->setProgress( (progress += 0.0001) );
		}
#elif DEBUG_MODEL_AND_PROGRESS
		this->lines->add( new String( "Debugging model and progress!" ) );

		for ( unsigned int i=0; i < 10000; i++ )
		{
			this->lines->add( new FormattedString( "progress: %f", progress ) );
			this->setProgress( (progress += 0.0001) );
		}
#else
		Path path( location );
		File file( path );
		if ( file.exists() )
		{
			FileInputStream fis( file );
			LineParser lp( fis );

			while ( !lp.hasFinished() )
			{
				this->lock( "TextViewerModel::initialize()" );
				{
					this->lines->add( lp.readLine() );
				}
				this->unlock( "TextViewerModel::initialize()" );
				this->setProgress( (progress += 0.02) );
			}
			if ( progress < 0.85 )
			{
				this->setProgress( 0.85 );
			}

			fis.close();
		} else {
			this->lines->add( new String( "Could not find file!" ) );
		}
#endif

	} catch ( ... ) {
		fprintf( stderr, "TextViewerModel::load: caught unexpected exception! Aborting!\n" );
		abort();
	}
	this->setProgress( 1.0 );
	this->setChanged( "TextViewerModel.data" );
}

String*
TextViewerModel::getData() const
{
	String* str = null;

	this->lock( "TextViewerModel::getData()" );
	{
		if ( this->lines )
		{
			StringBuffer sb;
			unsigned int max = this->lines->getLength();
			for ( unsigned int i=0; i < max; i++ )
			{
				sb.append( dynamic_cast<const String&>( this->lines->elementAt( i ) ) );
				sb.append( '\n' );
			}
			str = sb.toString();
		} else {
			str = new String( "Loading..." );
		}
	}
	this->unlock( "TextViewerModel::getData()" );
	
	return str;
}
