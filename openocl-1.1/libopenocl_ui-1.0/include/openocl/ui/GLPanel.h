/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_GLPANEL_H
#define OPENOCL_UI_GLPANEL_H

#include "openocl/ui/Panel.h"

namespace openocl {
  namespace ui {

class GLPanel : public openocl::ui::Panel
{
public:
  GLPanel( Panel* aPanel );

private:
  Panel* panel;
  unsigned int glid;
};

};};

#endif
