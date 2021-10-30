#ifndef OPENOCL_TEXTVIEWER_TEXTVIEWERDOC_H
#define OPENOCL_TEXTVIEWER_TEXTVIEWERDOC_H

#include "openocl/textviewer.h"

#include <openocl/application/Document.h>
#include <openocl/base.h>
#include <openocl/io.h>

namespace openocl {
	namespace textviewer {

class TextViewerDoc : public openocl::application::Document
{
public:
	TextViewerDoc();
	~TextViewerDoc();

	//	public virtual methods (Document)
	virtual void init( const openocl::base::Values& values );
	virtual void saveDocument();
	virtual void saveDocument( int typeId );
	virtual void exportDocument( const openocl::base::String& location, int typeId );
	virtual bool handleActionEvent( const openocl::base::event::ActionEvent& anActionEvent );

protected:
	virtual bool run();

private:	// methods
	void load();

private:
	TextViewerModel* model;
	TextViewerView*  view;
};

};};

#endif
