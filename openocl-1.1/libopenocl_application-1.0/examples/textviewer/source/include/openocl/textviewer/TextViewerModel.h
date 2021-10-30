#ifndef OPENOCL_TEXTVIEWER_TEXTVIEWERMODEL_H
#define OPENOCL_TEXTVIEWER_TEXTVIEWERMODEL_H

#include "openocl/textviewer.h"

#include <openocl/base.h>
#include <openocl/base/event.h>
#include <openocl/io.h>
#include <openocl/mvc/Model.h>
#include <openocl/util.h>

namespace openocl {
	namespace textviewer {

class TextViewerModel : public openocl::mvc::Model
{
public:
	TextViewerModel();
	~TextViewerModel();
	
	//	public virtual methods (EventListener)
	void deliverEvent( const openocl::base::event::Event& anEvent );

	//	public async methods
	void load( const openocl::base::String& location );
	openocl::base::String* getData() const;

private:
	openocl::util::Sequence* lines;
};

};};

#endif
