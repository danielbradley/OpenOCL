#ifndef OPENOCL_TEXTVIEWER_TEXTVIEWERCONTROL_H
#define OPENOCL_TEXTVIEWER_TEXTVIEWERCONTROL_H

#include "openocl/textviewer.h"

#include <openocl/base.h>
#include <openocl/ui.h>
#include <openocl/ui/Control.h>

namespace openocl {
	namespace textviewer {

class TextViewerControl : public openocl::ui::Control
{
public:
	TextViewerControl( TextViewerModel& aModel );
	~TextViewerControl();

	//	public virtual methods (Control)
	virtual bool handleActionEvent( const openocl::base::event::ActionEvent& anEvent );
	virtual void refresh( const openocl::base::String& id );

private:
	openocl::ui::Container*    base;
};

};};

#endif
