/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_H
#define OPENOCL_UI_H

#include <openocl/types.h>
#include <openocl/threads.h>
#include <openocl/imaging.h>
#include <openocl/util.h>

namespace openocl {
  namespace ui {

/**
 *
 *                Object
 *                  |
 *              Component
 *              |       |
 *          Button  Container<>---Components
 *                  |      |
 *                  |    ToolBar
 *                  |
 *  Frame<>-------Panel
 *       <>-------AbstractDisplay
 */   

class PUBLIC Visual;
class PUBLIC Screen;

class PUBLIC Dimensions;
class PUBLIC Drawable;
	class PUBLIC OffscreenImage;
	class PUBLIC Panel;

class PUBLIC ApplicationMenu;

/**
 *  The window class represents a window or subwindow of an
 *  application.
 */

/**
 *  A component is a graphical element, eg. a toolbar.
 */
class PUBLIC Component;
	class PUBLIC Blank;
	class PUBLIC Canvas;
		class PUBLIC TextCanvas;
	class PUBLIC CheckBox;
	class PUBLIC ComboBox;
	class PUBLIC Container;
		class PUBLIC ComponentGroup;
		class PUBLIC IconGroup;
		class PUBLIC OpaqueContainer;
			class PUBLIC ButtonGroup;
			class PUBLIC ComboGroup;
			class PUBLIC MenuGroup;
		class PUBLIC Pane;
			class PUBLIC DocumentPane;
			class PUBLIC MenuPane;
			class PUBLIC PanelScrollPane;
			class PUBLIC ScrollPane;
		class PUBLIC SplitView;
		class PUBLIC TabbedView;
		class PUBLIC ToolBar;
		class PUBLIC ToolBarGroup;
	class PUBLIC Control;
	class PUBLIC ExtIcon;
	class PUBLIC Icon;
	class PUBLIC Menu;
	class PUBLIC MenuItem;
		class PUBLIC RuntimeSummaryItem;
	class PUBLIC PanelScroller;
	class PUBLIC ProgressIndicator;
	class PUBLIC Scroller;
	class PUBLIC ScrollBar;
	class PUBLIC ScrollingCanvas;
	class PUBLIC Selector;
	class PUBLIC SplitViewManager;
	class PUBLIC StatusBar;
	class PUBLIC Textual;
		class PUBLIC Button;
		class PUBLIC IconText;
		class PUBLIC Label;
		class PUBLIC NumberField;
		class PUBLIC TextArea;
		class PUBLIC TextField;

/**
 *  The frame class represents a traditional parent window.
 */
class PUBLIC Frame;
	class PUBLIC ApplicationFrame;
	class PUBLIC FloatingFrame;
	class PUBLIC PopupFrame;
		class PUBLIC Dialog;
		class PUBLIC PopupMenu;
	class PUBLIC WindowFrame;

class PUBLIC FocusManager;
class PUBLIC Font;
class PUBLIC FontManager;
class PUBLIC PanelGeometry;

/**
 *  A graphics context represents information about the rasterizing
 *  environment that is collected together for convenience, eg.
 *  colors etc.
 */
class PUBLIC ClipArea;
class PUBLIC GC3d;
class PUBLIC Graphical;
class PUBLIC GraphicsBase;
class PUBLIC GraphicsContext;

class PUBLIC LayoutManager;
	class PUBLIC AbsoluteLayout;
	class PUBLIC ArrangedLayout;
	class PUBLIC BorderLayout;
	class PUBLIC ColumnLayout;
	class PUBLIC DirectedLayout;
	class PUBLIC GraphLayout;
	class PUBLIC GridLayout;
	class PUBLIC SidewaysLayout;
	class PUBLIC SimpleLayout;
	class PUBLIC StackedLayout;
	class PUBLIC TableLayout;

class PUBLIC OffscreenImageManager;
class PUBLIC Region;

};};

#endif
