/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/UIException.h"

#include <openocl/base/Exception.h>
#include <openocl/base/String.h>
#include <openocl/base/StringBuffer.h>

using namespace openocl::base;
using namespace openocl::ui;

static const String EXCEPTION_TYPE( "openocl::ui::UIException" );

UIException::UIException()
{
}

UIException::UIException( const String* aMessage )
: Exception( aMessage )
{}

UIException::UIException( const String& aMessage )
: Exception( aMessage )
{}

UIException::UIException( const StringBuffer& aMessage )
: Exception( aMessage )
{}

UIException::UIException( const Exception* anException, const String* aMessage )
: Exception( anException, aMessage )
{}

UIException::UIException( const Exception* anException, const String& aMessage )
: Exception( anException, aMessage )
{}

UIException::~UIException()
{}

const String&
UIException::getType() const
{
  return EXCEPTION_TYPE;
}
