#include "openocl/textviewer/TextViewerControl.h"
#include "openocl/textviewer/TextViewerModel.h"
#include "openocl/textviewer/TextViewerView.h"

#include <openocl/base/String.h>
#include <openocl/base/StringBuffer.h>
#include <openocl/base/User.h>
#include <openocl/controls/ShortcutBox.h>
#include <openocl/io/Path.h>
#include <openocl/ui/Label.h>

using namespace openocl::base;
using namespace openocl::controls;
using namespace openocl::io;
using namespace openocl::ui;
using namespace openocl::textviewer;

TextViewerView::TextViewerView( TextViewerModel& aModel ) : View( aModel ), model( aModel )
{
	this->ctrl = new TextViewerControl( aModel );
	this->addViewComponent( *this->ctrl );
}

TextViewerView::~TextViewerView()
{
	this->removeViewComponent( *this->ctrl );
	delete this->ctrl;
}

void
TextViewerView::refresh( const String& id )
{
	this->ctrl->refresh( id );
}
