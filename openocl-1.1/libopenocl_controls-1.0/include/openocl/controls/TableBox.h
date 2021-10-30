/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_CONTROLS_TABLEBOX_H
#define OPENOCL_CONTROLS_TABLEBOX_H

#include <openocl/base/event.h>
#include <openocl/controls.h>
#include <openocl/data.h>
#include <openocl/ui/Control.h>
#include <openocl/ui.h>

namespace openocl {
	namespace controls {

class TableBox : public openocl::ui::Control
{
public:
	TableBox( openocl::data::TableData& data );
	virtual ~TableBox();

	//	public virtual methods (Control)
	virtual bool handleActionEvent( const openocl::base::event::ActionEvent& anActionEvent );
	virtual void selectRow( int row );
	virtual void refresh();

	void setOffsets( int x, int y );
	int getOffsetOfRow( int row ) const;

private:
	openocl::ui::Container* base;

};

};};

#endif
