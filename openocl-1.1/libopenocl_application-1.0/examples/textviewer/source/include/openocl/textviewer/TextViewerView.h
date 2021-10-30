#ifndef OPENOCL_TEXTVIEWER_TEXTVIEWERVIEW_H
#define OPENOCL_TEXTVIEWER_TEXTVIEWERVIEW_H

#include "openocl/textviewer.h"

#include <openocl/mvc/View.h>
#include <openocl/controls.h>

namespace openocl {
	namespace textviewer {

class TextViewerView : public openocl::mvc::View
{
public:
	TextViewerView( TextViewerModel& aModel );
	~TextViewerView();

	//	public virtual methods (View)
	void refresh( const openocl::base::String& id );

private:
	TextViewerModel& model;

	TextViewerControl* ctrl;
};

};};

#endif
