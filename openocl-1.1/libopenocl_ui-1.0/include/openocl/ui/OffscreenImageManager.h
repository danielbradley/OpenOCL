/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_OFFSCREENIMAGEMANAGER_H
#define OPENOCL_UI_OFFSCREENIMAGEMANAGER_H

#include "openocl/ui.h"
#include <openocl/base/NoSuchElementException.h>
#include <openocl/base/Object.h>
#include <openocl/imaging.h>
#include <openocl/util.h>

namespace openocl {
	namespace ui {

class OffscreenImageManager : public openocl::base::Object
{
public:
	OffscreenImageManager();
	virtual ~OffscreenImageManager();
	
	void loadDefaultImages( const openocl::ui::GraphicsContext& gc );
	
	void addImage( const openocl::base::String& anImageId, openocl::imaging::Image* anImage );
	void cacheImages( const GraphicsContext& gc );
	
	const OffscreenImage& findImage( const GraphicsContext& gc, const openocl::base::String& imageId )
		throw (openocl::base::NoSuchElementException*);

private:
	openocl::util::Dictionary* images;
	openocl::util::Dictionary* cache;

};

};};

#endif
