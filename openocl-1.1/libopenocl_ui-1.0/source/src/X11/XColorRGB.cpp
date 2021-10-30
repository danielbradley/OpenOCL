/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/X11/XColorRGB.h"
#include <openocl/imaging/Color.h>

using namespace openocl::base;
using namespace openocl::imaging;
using namespace openocl::ui::X11;

XColorRGB::XColorRGB( const Color& aColor )
{
	unsigned long color = 0;
	color |= (((byte) aColor.getRed()) << 16);
	color |= (((byte) aColor.getGreen()) << 8);
	color |= ((byte) aColor.getBlue());

	this->color = color;
}

unsigned short int
XColorRGB::getRed() const
{
	return (0xFF0000 & this->color) >> 16;
}

unsigned short int
XColorRGB::getGreen() const
{
	return (0x00FF00 & this->color) >> 8;
}

unsigned short int
XColorRGB::getBlue() const
{
	return (0x0000FF & this->color);
}

unsigned long
XColorRGB::getColor() const
{
	return this->color;
}
