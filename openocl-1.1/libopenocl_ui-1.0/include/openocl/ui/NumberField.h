/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_NUMBERFIELD_H
#define OPENOCL_UI_NUMBERFIELD_H

#include "openocl/ui/Region.h"
#include "openocl/ui/TextField.h"
#include "openocl/ui.h"

namespace openocl {
	namespace ui {

class NumberField : public TextField
{
public:
	NumberField( const openocl::base::String& defaultValue );

	~NumberField();
	virtual void processEvent( const openocl::base::event::Event& anEvent );
	virtual void doLayout( const GraphicsContext& gc );

	//	public virtual methods (TextField)
	virtual void setData( const openocl::base::String& aString );

	//	public methods
	void setMinValue( double aValue );
	void setValue( double aValue );
	void setMaxValue( double aValue );
	void setIncrement( double aValue );
	void setAsFloat( bool asFloat );

	//	public virtual constant methods
	virtual bool draw( GraphicsContext& gc ) const;
	
private:	
	void decrementValue();
	void incrementValue();

	Region decreaseRegion;
	Region increaseRegion;

	bool decPressed;
	bool incPressed;
	bool hover;

	double min;
	double value;
	double max;
	double inc;
	
	bool asFloat;
};

};};

#endif
