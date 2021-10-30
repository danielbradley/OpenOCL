/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_X11_XCOLORRGB_H
#define OPENOCL_UI_X11_XCOLORRGB_H 

#include <openocl/base/Object.h>
#include <openocl/imaging.h>

namespace openocl {
  namespace ui {
    namespace X11 {

class XColorRGB : public openocl::base::Object
{
public:
	XColorRGB( const openocl::imaging::Color& aColor );
	
	unsigned short int getRed() const;
	unsigned short int getGreen() const;
	unsigned short int getBlue() const;
	
	unsigned long getColor() const;

private:
	unsigned long color;
};

};};};

#endif
