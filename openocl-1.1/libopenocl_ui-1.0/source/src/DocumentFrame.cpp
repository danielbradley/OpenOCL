/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/DocumentFrame.h"

#include "openocl/ui/Panel.h"
#include "openocl/ui/Screen.h"
#include "openocl/ui/Visual.h"

using namespace openocl::ui;

DocumentFrame::DocumentFrame( Visual& aVisual ) : Frame( aVisual.getDefaultScreen().createPanel( Panel::DOCUMENT ) )
{
}
