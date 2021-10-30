/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_UIEXCEPTION_H
#define OPENOCL_UI_UIEXCEPTION_H

#include <openocl/base/Exception.h>
#include <openocl/base.h>

namespace openocl {
    namespace ui {

class UIException : public openocl::base::Exception
{
public:
  UIException();
  UIException( const openocl::base::String* aMessage );
  UIException( const openocl::base::String& aMessage );
  UIException( const openocl::base::StringBuffer& aMessage );
  UIException( const openocl::base::Exception* anException, const openocl::base::String* aMessage );
  UIException( const openocl::base::Exception* anException, const openocl::base::String& aMessage );
  virtual ~UIException();

  virtual const openocl::base::String& getType() const;
};

};};

#endif
