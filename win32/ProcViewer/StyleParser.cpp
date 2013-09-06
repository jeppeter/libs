#include "stdafx.h"
#include "StyleParser.h"

// Class style mapping
StyleMapper g_ClassStyleMapper[] 
                                    =   {   MAKE_STYLE_MAPPER_ITEM( CS_VREDRAW ),
                                            MAKE_STYLE_MAPPER_ITEM( CS_HREDRAW ),
                                            MAKE_STYLE_MAPPER_ITEM( CS_DBLCLKS ),
                                            MAKE_STYLE_MAPPER_ITEM( CS_OWNDC ),
                                            MAKE_STYLE_MAPPER_ITEM( CS_CLASSDC ),
                                            MAKE_STYLE_MAPPER_ITEM( CS_PARENTDC ),
                                            MAKE_STYLE_MAPPER_ITEM( CS_NOCLOSE ),
                                            MAKE_STYLE_MAPPER_ITEM( CS_SAVEBITS ),
                                            MAKE_STYLE_MAPPER_ITEM( CS_BYTEALIGNCLIENT ),
                                            MAKE_STYLE_MAPPER_ITEM( CS_BYTEALIGNWINDOW ),
                                            MAKE_STYLE_MAPPER_ITEM( CS_GLOBALCLASS ),
                                            MAKE_STYLE_MAPPER_ITEM( CS_IME ),    
                                            MAKE_STYLE_MAPPER_ITEM( CS_DROPSHADOW ),
                                            { 0, 0 }};

// Window "Ex" style mapping
StyleMapper g_WndExStyleMapper[] 
                                    =   {   MAKE_STYLE_MAPPER_ITEM( WS_EX_DLGMODALFRAME ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_EX_NOPARENTNOTIFY ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_EX_TOPMOST ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_EX_ACCEPTFILES ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_EX_TRANSPARENT ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_EX_MDICHILD ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_EX_TOOLWINDOW ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_EX_WINDOWEDGE ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_EX_CLIENTEDGE ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_EX_CONTEXTHELP ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_EX_RIGHT ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_EX_RTLREADING ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_EX_LTRREADING ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_EX_LEFTSCROLLBAR ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_EX_CONTROLPARENT ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_EX_STATICEDGE ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_EX_APPWINDOW ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_EX_OVERLAPPEDWINDOW ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_EX_PALETTEWINDOW ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_EX_LAYERED ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_EX_NOINHERITLAYOUT ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_EX_LAYOUTRTL ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_EX_COMPOSITED ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_EX_NOACTIVATE ),
                                            { 0, 0 }};

// Normal window style mapping
StyleMapper g_WndStyleMapper[]  
                                    =   {   MAKE_STYLE_MAPPER_ITEM( WS_OVERLAPPED ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_POPUP ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_CHILD ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_MINIMIZE ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_VISIBLE ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_DISABLED ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_CLIPSIBLINGS ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_CLIPCHILDREN ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_MAXIMIZE ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_CAPTION ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_BORDER ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_DLGFRAME ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_VSCROLL ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_HSCROLL ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_SYSMENU ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_THICKFRAME ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_GROUP ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_TABSTOP ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_OVERLAPPEDWINDOW ),
                                            MAKE_STYLE_MAPPER_ITEM( WS_POPUPWINDOW ),
                                            { 0, 0 }};

// Dialog styles
StyleMapper g_DlgStyleMapper[] 
                                    =   {   MAKE_STYLE_MAPPER_ITEM( DS_ABSALIGN ),
                                            MAKE_STYLE_MAPPER_ITEM( DS_SYSMODAL ),
                                            MAKE_STYLE_MAPPER_ITEM( DS_LOCALEDIT ),
                                            MAKE_STYLE_MAPPER_ITEM( DS_SETFONT ),
                                            MAKE_STYLE_MAPPER_ITEM( DS_MODALFRAME ),
                                            MAKE_STYLE_MAPPER_ITEM( DS_NOIDLEMSG ),
                                            MAKE_STYLE_MAPPER_ITEM( DS_SETFOREGROUND ),
                                            MAKE_STYLE_MAPPER_ITEM( DS_3DLOOK ),
                                            MAKE_STYLE_MAPPER_ITEM( DS_FIXEDSYS ),
                                            MAKE_STYLE_MAPPER_ITEM( DS_NOFAILCREATE ),
                                            MAKE_STYLE_MAPPER_ITEM( DS_CONTROL ),
                                            MAKE_STYLE_MAPPER_ITEM( DS_CENTER ),
                                            MAKE_STYLE_MAPPER_ITEM( DS_CENTERMOUSE ),
                                            MAKE_STYLE_MAPPER_ITEM( DS_CONTEXTHELP ),
                                            MAKE_STYLE_MAPPER_ITEM( DS_SHELLFONT ),
                                            #if(_WIN32_WCE >= 0x0500)
                                                MAKE_STYLE_MAPPER_ITEM( DS_USEPIXELS ),
                                            #endif
                                            { 0, 0 }};

// Edit control styles
StyleMapper g_EdtStyleMapper[] 
                                    =   {   MAKE_STYLE_MAPPER_ITEM( ES_LEFT ),
                                            MAKE_STYLE_MAPPER_ITEM( ES_CENTER ),
                                            MAKE_STYLE_MAPPER_ITEM( ES_RIGHT ),
                                            MAKE_STYLE_MAPPER_ITEM( ES_MULTILINE ),
                                            MAKE_STYLE_MAPPER_ITEM( ES_UPPERCASE ),
                                            MAKE_STYLE_MAPPER_ITEM( ES_LOWERCASE ),
                                            MAKE_STYLE_MAPPER_ITEM( ES_PASSWORD ),
                                            MAKE_STYLE_MAPPER_ITEM( ES_AUTOVSCROLL ),
                                            MAKE_STYLE_MAPPER_ITEM( ES_AUTOHSCROLL ),
                                            MAKE_STYLE_MAPPER_ITEM( ES_NOHIDESEL ),
                                            MAKE_STYLE_MAPPER_ITEM( ES_OEMCONVERT ),
                                            MAKE_STYLE_MAPPER_ITEM( ES_READONLY ),
                                            MAKE_STYLE_MAPPER_ITEM( ES_WANTRETURN ),
                                            #if( WINVER >= 0x0400 )
                                                MAKE_STYLE_MAPPER_ITEM( ES_NUMBER ),
                                            #endif
                                            { 0, 0 }};

// Normal list control styles
StyleMapper g_ListCtrlStyleMapper[] 
                                            =   {   MAKE_STYLE_MAPPER_ITEM( LVS_ICON ),
                                                    MAKE_STYLE_MAPPER_ITEM( LVS_REPORT ),
                                                    MAKE_STYLE_MAPPER_ITEM( LVS_SMALLICON ),
                                                    MAKE_STYLE_MAPPER_ITEM( LVS_LIST ),
                                                    MAKE_STYLE_MAPPER_ITEM( LVS_TYPEMASK ),
                                                    MAKE_STYLE_MAPPER_ITEM( LVS_SINGLESEL ),
                                                    MAKE_STYLE_MAPPER_ITEM( LVS_SHOWSELALWAYS ),
                                                    MAKE_STYLE_MAPPER_ITEM( LVS_SORTASCENDING ),
                                                    MAKE_STYLE_MAPPER_ITEM( LVS_SORTDESCENDING ),
                                                    MAKE_STYLE_MAPPER_ITEM( LVS_SHAREIMAGELISTS ),
                                                    MAKE_STYLE_MAPPER_ITEM( LVS_NOLABELWRAP ),
                                                    MAKE_STYLE_MAPPER_ITEM( LVS_AUTOARRANGE ),
                                                    MAKE_STYLE_MAPPER_ITEM( LVS_EDITLABELS ),
                                                    #if (_WIN32_IE >= 0x0300)
                                                        MAKE_STYLE_MAPPER_ITEM( LVS_OWNERDATA ),
                                                    #endif
                                                    MAKE_STYLE_MAPPER_ITEM( LVS_NOSCROLL ),
                                                    MAKE_STYLE_MAPPER_ITEM( LVS_TYPESTYLEMASK ),
                                                    MAKE_STYLE_MAPPER_ITEM( LVS_ALIGNTOP ),
                                                    MAKE_STYLE_MAPPER_ITEM( LVS_ALIGNLEFT ),
                                                    MAKE_STYLE_MAPPER_ITEM( LVS_ALIGNMASK ),
                                                    MAKE_STYLE_MAPPER_ITEM( LVS_OWNERDRAWFIXED ),
                                                    MAKE_STYLE_MAPPER_ITEM( LVS_NOCOLUMNHEADER ),
                                                    MAKE_STYLE_MAPPER_ITEM( LVS_NOSORTHEADER ),
                                                    { 0, 0 }};

// Extended list control styles
StyleMapper g_ListCtrlExStyleMapper[] 
                                            =   {   MAKE_STYLE_MAPPER_ITEM( LVS_EX_GRIDLINES ),
                                                    MAKE_STYLE_MAPPER_ITEM( LVS_EX_SUBITEMIMAGES ),
                                                    MAKE_STYLE_MAPPER_ITEM( LVS_EX_CHECKBOXES ),
                                                    MAKE_STYLE_MAPPER_ITEM( LVS_EX_TRACKSELECT ),
                                                    MAKE_STYLE_MAPPER_ITEM( LVS_EX_HEADERDRAGDROP ),
                                                    MAKE_STYLE_MAPPER_ITEM( LVS_EX_FULLROWSELECT ),
                                                    MAKE_STYLE_MAPPER_ITEM( LVS_EX_ONECLICKACTIVATE ),
                                                    MAKE_STYLE_MAPPER_ITEM( LVS_EX_TWOCLICKACTIVATE ),
                                                    #if( _WIN32_IE >= 0x0400 )
                                                        MAKE_STYLE_MAPPER_ITEM( LVS_EX_FLATSB ),
                                                        MAKE_STYLE_MAPPER_ITEM( LVS_EX_REGIONAL ),
                                                        MAKE_STYLE_MAPPER_ITEM( LVS_EX_INFOTIP ),
                                                        MAKE_STYLE_MAPPER_ITEM( LVS_EX_UNDERLINEHOT ),
                                                        MAKE_STYLE_MAPPER_ITEM( LVS_EX_UNDERLINECOLD ),
                                                        MAKE_STYLE_MAPPER_ITEM( LVS_EX_MULTIWORKAREAS ),
                                                    #endif
                                                    #if( _WIN32_IE >= 0x0500 )
                                                        MAKE_STYLE_MAPPER_ITEM( LVS_EX_LABELTIP ),
                                                        MAKE_STYLE_MAPPER_ITEM( LVS_EX_BORDERSELECT ),
                                                    #endif
                                                    #if( _WIN32_WINNT >= 0x501 )
                                                        MAKE_STYLE_MAPPER_ITEM( LVS_EX_DOUBLEBUFFER ),
                                                        MAKE_STYLE_MAPPER_ITEM( LVS_EX_HIDELABELS ),
                                                        MAKE_STYLE_MAPPER_ITEM( LVS_EX_SINGLEROW ),
                                                        MAKE_STYLE_MAPPER_ITEM( LVS_EX_SNAPTOGRID ),
                                                        MAKE_STYLE_MAPPER_ITEM( LVS_EX_SIMPLESELECT ),
                                                    #endif 
                                                    { 0, 0 }};

// Listbox styles
StyleMapper g_ListBoxStyleMapper[]    
                                            =   {   MAKE_STYLE_MAPPER_ITEM( LBS_NOTIFY ),
                                                    MAKE_STYLE_MAPPER_ITEM( LBS_SORT ),
                                                    MAKE_STYLE_MAPPER_ITEM( LBS_NOREDRAW ),
                                                    MAKE_STYLE_MAPPER_ITEM( LBS_MULTIPLESEL ),
                                                    MAKE_STYLE_MAPPER_ITEM( LBS_OWNERDRAWFIXED ),
                                                    MAKE_STYLE_MAPPER_ITEM( LBS_OWNERDRAWVARIABLE ),
                                                    MAKE_STYLE_MAPPER_ITEM( LBS_HASSTRINGS ),
                                                    MAKE_STYLE_MAPPER_ITEM( LBS_USETABSTOPS ),
                                                    MAKE_STYLE_MAPPER_ITEM( LBS_NOINTEGRALHEIGHT ),
                                                    MAKE_STYLE_MAPPER_ITEM( LBS_MULTICOLUMN ),
                                                    MAKE_STYLE_MAPPER_ITEM( LBS_WANTKEYBOARDINPUT ),
                                                    MAKE_STYLE_MAPPER_ITEM( LBS_EXTENDEDSEL ),
                                                    MAKE_STYLE_MAPPER_ITEM( LBS_DISABLENOSCROLL ),
                                                    MAKE_STYLE_MAPPER_ITEM( LBS_NODATA ),
                                                    #if(WINVER >= 0x0400)
                                                        MAKE_STYLE_MAPPER_ITEM( LBS_NOSEL ),
                                                    #endif
                                                    MAKE_STYLE_MAPPER_ITEM( LBS_COMBOBOX ),
                                                    MAKE_STYLE_MAPPER_ITEM( LBS_STANDARD ),
                                                    { 0, 0 }};

// Common control styles
StyleMapper g_CommonCtrlStyleMapper[] 
                                            =   {   MAKE_STYLE_MAPPER_ITEM( CCS_TOP ),
                                                    MAKE_STYLE_MAPPER_ITEM( CCS_NOMOVEY ),
                                                    MAKE_STYLE_MAPPER_ITEM( CCS_BOTTOM ),
                                                    MAKE_STYLE_MAPPER_ITEM( CCS_NORESIZE ),
                                                    MAKE_STYLE_MAPPER_ITEM( CCS_NOPARENTALIGN ),
                                                    MAKE_STYLE_MAPPER_ITEM( CCS_ADJUSTABLE ),
                                                    MAKE_STYLE_MAPPER_ITEM( CCS_NODIVIDER ),
                                                    #if (_WIN32_IE >= 0x0300)
                                                        MAKE_STYLE_MAPPER_ITEM( CCS_VERT ),
                                                        MAKE_STYLE_MAPPER_ITEM( CCS_LEFT ),
                                                        MAKE_STYLE_MAPPER_ITEM( CCS_RIGHT ),
                                                        MAKE_STYLE_MAPPER_ITEM( CCS_NOMOVEX ),
                                                    #endif
                                                    { 0, 0 }};

// Status bar styles
StyleMapper g_StatusBarStyleMapper[]  
                                            =   {   MAKE_STYLE_MAPPER_ITEM( SBARS_SIZEGRIP ),
                                                    MAKE_STYLE_MAPPER_ITEM( SBARS_TOOLTIPS ),
                                                    MAKE_STYLE_MAPPER_ITEM( SBT_NOBORDERS ),
                                                    #if (_WIN32_IE >= 0x0500)
                                                        MAKE_STYLE_MAPPER_ITEM( SBT_NOTABPARSING ),
                                                    #endif
                                                    MAKE_STYLE_MAPPER_ITEM( SBT_OWNERDRAW ),
                                                    MAKE_STYLE_MAPPER_ITEM( SBT_POPOUT ),
                                                    MAKE_STYLE_MAPPER_ITEM( SBT_RTLREADING ),
                                                    MAKE_STYLE_MAPPER_ITEM( SBT_TOOLTIPS ),
                                                    { 0, 0 }};

// Button style mapper
StyleMapper g_ButtonStyleMapper[]     
                                            =   {   MAKE_STYLE_MAPPER_ITEM( BS_PUSHBUTTON ),
                                                    MAKE_STYLE_MAPPER_ITEM( BS_DEFPUSHBUTTON ),
                                                    MAKE_STYLE_MAPPER_ITEM( BS_CHECKBOX ),
                                                    MAKE_STYLE_MAPPER_ITEM( BS_AUTOCHECKBOX ),
                                                    MAKE_STYLE_MAPPER_ITEM( BS_RADIOBUTTON ),
                                                    MAKE_STYLE_MAPPER_ITEM( BS_3STATE ),
                                                    MAKE_STYLE_MAPPER_ITEM( BS_AUTO3STATE ),
                                                    MAKE_STYLE_MAPPER_ITEM( BS_GROUPBOX ),
                                                    MAKE_STYLE_MAPPER_ITEM( BS_USERBUTTON ),
                                                    MAKE_STYLE_MAPPER_ITEM( BS_AUTORADIOBUTTON ),
                                                    MAKE_STYLE_MAPPER_ITEM( BS_PUSHBOX ),
                                                    MAKE_STYLE_MAPPER_ITEM( BS_OWNERDRAW ),
                                                    MAKE_STYLE_MAPPER_ITEM( BS_TYPEMASK ),
                                                    MAKE_STYLE_MAPPER_ITEM( BS_LEFTTEXT ),
                                                    #if(WINVER >= 0x0400)
                                                        MAKE_STYLE_MAPPER_ITEM( BS_TEXT ),
                                                        MAKE_STYLE_MAPPER_ITEM( BS_ICON ),
                                                        MAKE_STYLE_MAPPER_ITEM( BS_BITMAP ),
                                                        MAKE_STYLE_MAPPER_ITEM( BS_LEFT ), 
                                                        MAKE_STYLE_MAPPER_ITEM( BS_RIGHT ),
                                                        MAKE_STYLE_MAPPER_ITEM( BS_CENTER ),
                                                        MAKE_STYLE_MAPPER_ITEM( BS_TOP ),
                                                        MAKE_STYLE_MAPPER_ITEM( BS_BOTTOM ),
                                                        MAKE_STYLE_MAPPER_ITEM( BS_VCENTER ), 
                                                        MAKE_STYLE_MAPPER_ITEM( BS_PUSHLIKE ),
                                                        MAKE_STYLE_MAPPER_ITEM( BS_MULTILINE ),
                                                        MAKE_STYLE_MAPPER_ITEM( BS_NOTIFY ),
                                                        MAKE_STYLE_MAPPER_ITEM( BS_FLAT ),
                                                        MAKE_STYLE_MAPPER_ITEM( BS_RIGHTBUTTON ),
                                                    #endif
                                                    { 0, 0 }};

// Styles for static control
extern StyleMapper g_StaticCtrlStyleMapper[]
                                            =   {   MAKE_STYLE_MAPPER_ITEM( SS_LEFT ),
                                                    MAKE_STYLE_MAPPER_ITEM( SS_CENTER ),
                                                    MAKE_STYLE_MAPPER_ITEM( SS_RIGHT ),
                                                    MAKE_STYLE_MAPPER_ITEM( SS_ICON ),
                                                    MAKE_STYLE_MAPPER_ITEM( SS_BLACKRECT ),
                                                    MAKE_STYLE_MAPPER_ITEM( SS_GRAYRECT ),
                                                    MAKE_STYLE_MAPPER_ITEM( SS_WHITERECT ),
                                                    MAKE_STYLE_MAPPER_ITEM( SS_BLACKFRAME ),
                                                    MAKE_STYLE_MAPPER_ITEM( SS_GRAYFRAME ),
                                                    MAKE_STYLE_MAPPER_ITEM( SS_WHITEFRAME ),
                                                    MAKE_STYLE_MAPPER_ITEM( SS_USERITEM ),
                                                    MAKE_STYLE_MAPPER_ITEM( SS_SIMPLE ),
                                                    MAKE_STYLE_MAPPER_ITEM( SS_LEFTNOWORDWRAP ),
                                                    #if ( WINVER >= 0x0400)
                                                        MAKE_STYLE_MAPPER_ITEM( SS_OWNERDRAW ),
                                                        MAKE_STYLE_MAPPER_ITEM( SS_BITMAP ),
                                                        MAKE_STYLE_MAPPER_ITEM( SS_ENHMETAFILE ),
                                                        MAKE_STYLE_MAPPER_ITEM( SS_ETCHEDHORZ ),
                                                        MAKE_STYLE_MAPPER_ITEM( SS_ETCHEDVERT ),
                                                        MAKE_STYLE_MAPPER_ITEM( SS_ETCHEDFRAME ),
                                                        MAKE_STYLE_MAPPER_ITEM( SS_TYPEMASK ),
                                                    #endif 
                                                    #if( WINVER >= 0x0501 )
                                                        MAKE_STYLE_MAPPER_ITEM( SS_REALSIZECONTROL ),
                                                    #endif 
                                                    MAKE_STYLE_MAPPER_ITEM( SS_NOPREFIX ),
                                                    #if( WINVER >= 0x0400 )
                                                        MAKE_STYLE_MAPPER_ITEM( SS_NOTIFY ),
                                                        MAKE_STYLE_MAPPER_ITEM( SS_CENTERIMAGE ),
                                                        MAKE_STYLE_MAPPER_ITEM( SS_RIGHTJUST ),
                                                        MAKE_STYLE_MAPPER_ITEM( SS_REALSIZEIMAGE ),
                                                        MAKE_STYLE_MAPPER_ITEM( SS_SUNKEN ), 
                                                        MAKE_STYLE_MAPPER_ITEM( SS_EDITCONTROL ),
                                                        MAKE_STYLE_MAPPER_ITEM( SS_ENDELLIPSIS ),
                                                        MAKE_STYLE_MAPPER_ITEM( SS_PATHELLIPSIS ),
                                                        MAKE_STYLE_MAPPER_ITEM( SS_WORDELLIPSIS ),
                                                        MAKE_STYLE_MAPPER_ITEM( SS_ELLIPSISMASK ),
                                                    #endif
                                                    { 0, 0 }};

// Combo control styles
StyleMapper g_ComboCtrlStyleMapper[]
                                            =   {   MAKE_STYLE_MAPPER_ITEM( CBS_SIMPLE ),
                                                    MAKE_STYLE_MAPPER_ITEM( CBS_DROPDOWN ),
                                                    MAKE_STYLE_MAPPER_ITEM( CBS_DROPDOWNLIST ),
                                                    MAKE_STYLE_MAPPER_ITEM( CBS_OWNERDRAWFIXED ),
                                                    MAKE_STYLE_MAPPER_ITEM( CBS_OWNERDRAWVARIABLE ),
                                                    MAKE_STYLE_MAPPER_ITEM( CBS_AUTOHSCROLL ),
                                                    MAKE_STYLE_MAPPER_ITEM( CBS_OEMCONVERT ),
                                                    MAKE_STYLE_MAPPER_ITEM( CBS_SORT ),
                                                    MAKE_STYLE_MAPPER_ITEM( CBS_HASSTRINGS ),
                                                    MAKE_STYLE_MAPPER_ITEM( CBS_NOINTEGRALHEIGHT ),
                                                    MAKE_STYLE_MAPPER_ITEM( CBS_DISABLENOSCROLL ),
                                                    #if( WINVER >= 0x0400 ) 
                                                        MAKE_STYLE_MAPPER_ITEM( CBS_UPPERCASE ),
                                                        MAKE_STYLE_MAPPER_ITEM( CBS_LOWERCASE ),
                                                    #endif
                                                    { 0, 0 }};

// ComboEx extended style
StyleMapper g_ComboExCtrlStyleExMapper[]
                                            =   {   MAKE_STYLE_MAPPER_ITEM( CBES_EX_NOEDITIMAGE ),
                                                    MAKE_STYLE_MAPPER_ITEM( CBES_EX_NOEDITIMAGEINDENT ),
                                                    MAKE_STYLE_MAPPER_ITEM( CBES_EX_PATHWORDBREAKPROC ),
                                                    #if( _WIN32_IE >= 0x0400 )
                                                        MAKE_STYLE_MAPPER_ITEM( CBES_EX_NOSIZELIMIT ),
                                                        MAKE_STYLE_MAPPER_ITEM( CBES_EX_CASESENSITIVE ),
                                                    #endif
                                                    { 0, 0 }};

// For Scrollbars
StyleMapper g_ScrollbarStyleMapper[]
                                            =   {   MAKE_STYLE_MAPPER_ITEM( SBS_HORZ ),
                                                    MAKE_STYLE_MAPPER_ITEM( SBS_VERT ),
                                                    MAKE_STYLE_MAPPER_ITEM( SBS_TOPALIGN ),
                                                    MAKE_STYLE_MAPPER_ITEM( SBS_LEFTALIGN ),
                                                    MAKE_STYLE_MAPPER_ITEM( SBS_BOTTOMALIGN ),
                                                    MAKE_STYLE_MAPPER_ITEM( SBS_RIGHTALIGN ),
                                                    MAKE_STYLE_MAPPER_ITEM( SBS_SIZEBOXTOPLEFTALIGN ),
                                                    MAKE_STYLE_MAPPER_ITEM( SBS_SIZEBOXBOTTOMRIGHTALIGN ),
                                                    MAKE_STYLE_MAPPER_ITEM( SBS_SIZEBOX ),
                                                    #if( WINVER >= 0x0400 )
                                                        MAKE_STYLE_MAPPER_ITEM( SBS_SIZEGRIP ),
                                                    #endif
                                                    { 0, 0 }};

// For calendar/date time control
StyleMapper g_DateTimePickerStyleMapper[]
                                            =   {   MAKE_STYLE_MAPPER_ITEM( DTS_UPDOWN ),
                                                    MAKE_STYLE_MAPPER_ITEM( DTS_SHOWNONE ),
                                                    MAKE_STYLE_MAPPER_ITEM( DTS_SHORTDATEFORMAT ),
                                                    MAKE_STYLE_MAPPER_ITEM( DTS_LONGDATEFORMAT ),
                                                    #if( _WIN32_IE >= 0x500)
                                                        MAKE_STYLE_MAPPER_ITEM( DTS_SHORTDATECENTURYFORMAT ),
                                                    #endif
                                                    MAKE_STYLE_MAPPER_ITEM( DTS_TIMEFORMAT ),
                                                    MAKE_STYLE_MAPPER_ITEM( DTS_APPCANPARSE ),
                                                    MAKE_STYLE_MAPPER_ITEM( DTS_RIGHTALIGN ),
                                                    { 0, 0, }};

// Progress bar styles
StyleMapper g_ProgressBarStyleMapper[]
                                            =   {   MAKE_STYLE_MAPPER_ITEM( PBS_MARQUEE ),
                                                    MAKE_STYLE_MAPPER_ITEM( PBS_SMOOTH ),
                                                    MAKE_STYLE_MAPPER_ITEM( PBS_VERTICAL ),
                                                    { 0, 0 }};

// Tree control
StyleMapper g_TreeCtrlStyleMapper[]
                                            =   {   MAKE_STYLE_MAPPER_ITEM( TVS_HASBUTTONS ),
                                                    MAKE_STYLE_MAPPER_ITEM( TVS_HASLINES ),
                                                    MAKE_STYLE_MAPPER_ITEM( TVS_LINESATROOT ),
                                                    MAKE_STYLE_MAPPER_ITEM( TVS_EDITLABELS ),
                                                    MAKE_STYLE_MAPPER_ITEM( TVS_DISABLEDRAGDROP ),
                                                    MAKE_STYLE_MAPPER_ITEM( TVS_SHOWSELALWAYS ),
                                                    #if( _WIN32_IE >= 0x0300)
                                                        MAKE_STYLE_MAPPER_ITEM( TVS_RTLREADING ),
                                                        MAKE_STYLE_MAPPER_ITEM( TVS_NOTOOLTIPS ),
                                                        MAKE_STYLE_MAPPER_ITEM( TVS_CHECKBOXES ),
                                                        MAKE_STYLE_MAPPER_ITEM( TVS_TRACKSELECT ),
                                                        #if( _WIN32_IE >= 0x0400)
                                                            MAKE_STYLE_MAPPER_ITEM( TVS_SINGLEEXPAND ),
                                                            MAKE_STYLE_MAPPER_ITEM( TVS_INFOTIP ),
                                                            MAKE_STYLE_MAPPER_ITEM( TVS_FULLROWSELECT ),
                                                            MAKE_STYLE_MAPPER_ITEM( TVS_NOSCROLL ),
                                                            MAKE_STYLE_MAPPER_ITEM( TVS_NONEVENHEIGHT ),
                                                        #endif
                                                        #if (_WIN32_IE >= 0x500)
                                                            MAKE_STYLE_MAPPER_ITEM( TVS_NOHSCROLL ),
                                                        #endif
                                                    #endif
                                                    { 0, 0 }};

// For header control
StyleMapper g_HeaderCtrlStyleMapper[]
                                            =   {   MAKE_STYLE_MAPPER_ITEM( HDS_HORZ ),
                                                    MAKE_STYLE_MAPPER_ITEM( HDS_BUTTONS ),
                                                    #if( _WIN32_IE >= 0x0300 )
                                                        MAKE_STYLE_MAPPER_ITEM( HDS_HOTTRACK ),
                                                    #endif
                                                        MAKE_STYLE_MAPPER_ITEM( HDS_HIDDEN ),
                                                    #if (_WIN32_IE >= 0x0300)
                                                        MAKE_STYLE_MAPPER_ITEM( HDS_DRAGDROP ),
                                                        MAKE_STYLE_MAPPER_ITEM( HDS_FULLDRAG ),
                                                    #endif
                                                    #if (_WIN32_IE >= 0x0500)
                                                        MAKE_STYLE_MAPPER_ITEM( HDS_FILTERBAR ),
                                                    #endif
                                                    #if (_WIN32_WINNT >= 0x501)
                                                        MAKE_STYLE_MAPPER_ITEM( HDS_FLAT ),
                                                    #endif
                                                    { 0, 0 }};
// For header control
StyleMapper g_TabcontrolStyleMapper[]
                                            =   {   
                                                    #if( _WIN32_IE >= 0x0300 )
                                                        MAKE_STYLE_MAPPER_ITEM( TCS_SCROLLOPPOSITE ),
                                                        MAKE_STYLE_MAPPER_ITEM( TCS_BOTTOM ),
                                                        MAKE_STYLE_MAPPER_ITEM( TCS_RIGHT ),
                                                        MAKE_STYLE_MAPPER_ITEM( TCS_MULTISELECT ),
                                                    #endif
                                                    #if( _WIN32_IE >= 0x0400)
                                                        MAKE_STYLE_MAPPER_ITEM( TCS_FLATBUTTONS ),
                                                    #endif
                                                    MAKE_STYLE_MAPPER_ITEM( TCS_FORCEICONLEFT ),
                                                    MAKE_STYLE_MAPPER_ITEM( TCS_FORCELABELLEFT ),
                                                    #if( _WIN32_IE >= 0x0300)
                                                        MAKE_STYLE_MAPPER_ITEM( TCS_HOTTRACK ),
                                                        MAKE_STYLE_MAPPER_ITEM( TCS_VERTICAL ),
                                                    #endif
                                                    MAKE_STYLE_MAPPER_ITEM( TCS_TABS ),
                                                    MAKE_STYLE_MAPPER_ITEM( TCS_BUTTONS ),
                                                    MAKE_STYLE_MAPPER_ITEM( TCS_SINGLELINE ),
                                                    MAKE_STYLE_MAPPER_ITEM( TCS_MULTILINE ),
                                                    MAKE_STYLE_MAPPER_ITEM( TCS_RIGHTJUSTIFY ),
                                                    MAKE_STYLE_MAPPER_ITEM( TCS_FIXEDWIDTH ),
                                                    MAKE_STYLE_MAPPER_ITEM( TCS_RAGGEDRIGHT ),
                                                    MAKE_STYLE_MAPPER_ITEM( TCS_FOCUSONBUTTONDOWN ),
                                                    MAKE_STYLE_MAPPER_ITEM( TCS_OWNERDRAWFIXED ),
                                                    MAKE_STYLE_MAPPER_ITEM( TCS_TOOLTIPS ),
                                                    MAKE_STYLE_MAPPER_ITEM( TCS_FOCUSNEVER ),
                                                    { 0, 0 }};

// Extended styles for tab control
StyleMapper g_TabcontrolStyleExMapper[]
                                            =   {
                                                    #if (_WIN32_IE >= 0x0400)
                                                        MAKE_STYLE_MAPPER_ITEM( TCS_EX_FLATSEPARATORS ),
                                                        MAKE_STYLE_MAPPER_ITEM( TCS_EX_REGISTERDROP ),
                                                    #endif
                                                    { 0, 0 }};



//************************************
// Method:    StyleParser
// FullName:  StyleParser::StyleParser
// Access:    public 
// Returns:   
// Qualifier:
//************************************
StyleParser::StyleParser()
{
    InitClassNameMap();
    SortStyleMappers();
}

//************************************
// Method:    ~StyleParser
// FullName:  StyleParser::~StyleParser
// Access:    public 
// Returns:   
// Qualifier:
//************************************
StyleParser::~StyleParser()
{}

//************************************
// Method:    InitClassNameMap
// FullName:  StyleParser::InitClassNameMap
// Access:    protected 
// Returns:   void
// Qualifier:
//************************************
void StyleParser::InitClassNameMap()
{
    // Dialog style mapper
    m_ClassNameToStyleMapper[CString( _T( "#32770" ))]      = g_DlgStyleMapper;
    // Edit control style mapper
    m_ClassNameToStyleMapper[CString( WC_EDIT )]            = g_EdtStyleMapper;
    // Normal style of list control
    m_ClassNameToStyleMapper[CString( WC_LISTVIEW )]        = g_ListCtrlStyleMapper;
    // Normal styles for a listbox
    m_ClassNameToStyleMapper[CString( WC_LISTBOX )]         = g_ListBoxStyleMapper;
    // Control styles for status bar
    m_ClassNameToStyleMapper[CString( STATUSCLASSNAME )]    = g_StatusBarStyleMapper;
    // Normal styles for button
    m_ClassNameToStyleMapper[CString( WC_BUTTON )]          = g_ButtonStyleMapper;
    // Normal styles for static controls
    m_ClassNameToStyleMapper[CString( WC_STATIC )]          = g_StaticCtrlStyleMapper;
    // Normal styles for combo boxes
    m_ClassNameToStyleMapper[CString( WC_COMBOBOX )]        = g_ComboCtrlStyleMapper;
    // Normal styles for comboex controls
    m_ClassNameToStyleMapper[CString( WC_COMBOBOXEX )]      = g_ComboCtrlStyleMapper;
    // Scrollbar styles
    m_ClassNameToStyleMapper[CString( WC_SCROLLBAR )]       = g_ScrollbarStyleMapper;
    // Styles for date time control
    m_ClassNameToStyleMapper[CString( DATETIMEPICK_CLASS )] = g_DateTimePickerStyleMapper;
    // Progress bar class
    m_ClassNameToStyleMapper[CString( PROGRESS_CLASSW )]    = g_ProgressBarStyleMapper;
    // Tree control style mapper
    m_ClassNameToStyleMapper[CString( WC_TREEVIEW )]        = g_TreeCtrlStyleMapper;
    // Header control style
    m_ClassNameToStyleMapper[CString( WC_HEADER )]          = g_HeaderCtrlStyleMapper;
    // Tab control styles
    m_ClassNameToStyleMapper[CString( WC_TABCONTROL )]      = g_TabcontrolStyleMapper;

    // Extended style mapper for list control
    m_ClassNameToStyleExMapper[CString( WC_LISTVIEW )]      = g_ListCtrlExStyleMapper;
    // Extended style mapper for combo box ex
    m_ClassNameToStyleExMapper[CString( WC_COMBOBOXEX )]    = g_ComboExCtrlStyleExMapper;
    // Extended style mapper for tab control
    m_ClassNameToStyleExMapper[CString( WC_TABCONTROL )]    = g_TabcontrolStyleExMapper;
}// End InitClassNameMap

//************************************
// Method:    CompareFunc
// FullName:  StyleParser::CompareFunc
// Access:    protected static 
// Returns:   int __cdecl
// Qualifier:
// Parameter: const void * pFirst
// Parameter: const void * pSecond
//************************************
int __cdecl StyleParser::CompareFunc( const void* pFirst, const void* pSecond )
{
    // First style mapper
    const StyleMapper* pFirstStyleMapper    = RCAST( const StyleMapper*, pFirst );
    // Second style mapper
    const StyleMapper* pSecondStyleMapper   = RCAST( const StyleMapper*, pSecond );

    // Compare and return appropriate value
    if( pSecondStyleMapper->uStyle < pFirstStyleMapper->uStyle )
    {
        return -1;
    }
    else if ( pSecondStyleMapper->uStyle > pFirstStyleMapper->uStyle )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}// End CompareFunc

//************************************
// Method:    SortStyleMappers
// FullName:  StyleParser::SortStyleMappers
// Access:    protected 
// Returns:   void
// Qualifier:
//************************************
void StyleParser::SortStyleMappers()
{
    // First sort normal styles
    POSITION pPos = m_ClassNameToStyleMapper.GetStartPosition();
    while( pPos )
    {
        CString csClassName;
        StyleMapper* pStyleMapper = 0;

        // Get mapper at pos
        m_ClassNameToStyleMapper.GetNextAssoc( pPos, csClassName, (const StyleMapper*&)pStyleMapper );

        // Sort the mapper
        SortStyleMapper( pStyleMapper );
    }// End while

    // Now sort extended styles
    pPos = m_ClassNameToStyleExMapper.GetStartPosition();
    while( pPos )
    {
        CString csClassName;
        StyleMapper* pStyleMapper = 0;

        // Get mapper at pos
        m_ClassNameToStyleExMapper.GetNextAssoc( pPos, csClassName, (const StyleMapper*&)pStyleMapper );

        // Sort the mapper
        SortStyleMapper( pStyleMapper );
    }// End while
}// End SortStyleMappers

//************************************
// Method:    SortStyleMapper
// FullName:  StyleParser::SortStyleMapper
// Access:    protected 
// Returns:   void
// Qualifier:
// Parameter: StyleMapper * pStyleMapper_i
//************************************
void StyleParser::SortStyleMapper( StyleMapper* pStyleMapper_i )
{
    const DWORD dwStyleMapperItemCount = GetStyleMapperItemCount( pStyleMapper_i );
    qsort( pStyleMapper_i, dwStyleMapperItemCount, sizeof( StyleMapper ), CompareFunc );
}

//************************************
// Method:    GetStyleMapperItemCount
// FullName:  StyleParser::GetStyleMapperItemCount
// Access:    protected 
// Returns:   DWORD
// Qualifier:
// Parameter: const StyleMapper * pStyleMapper_i
//************************************
DWORD StyleParser::GetStyleMapperItemCount( const StyleMapper* pStyleMapper_i )
{
    // Get count of styles
    DWORD dwCount = 0;
    while( pStyleMapper_i && pStyleMapper_i[dwCount].lpctszStyle ) 
    {
        ++dwCount;
    }

    return dwCount;
}

//************************************
// Method:    GetClassStyleAsString
// FullName:  StyleParser::GetClassStyleAsString
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: const UINT uStyle_i
// Parameter: CString & csStyle_o
//************************************
void StyleParser::GetClassStyleAsString( const UINT uStyle_i, CString& csStyle_o ) const
{
    GetStylesAsFormattedString( uStyle_i, 
                                csStyle_o, 
                                g_ClassStyleMapper );
}

//************************************
// Method:    GetWindowStyleAsString
// FullName:  StyleParser::GetWindowStyleAsString
// Access:    public 
// Returns:   void
// Qualifier: const
// Parameter: const UINT uStyle_i
// Parameter: CString & csStyle_o
//************************************
void StyleParser::GetWindowStyleAsString( const UINT uStyle_i, CString& csStyle_o ) const
{
    GetStylesAsFormattedString( uStyle_i,
                                csStyle_o,
                                g_WndStyleMapper );
}

//************************************
// Method:    GetWindowStyleExAsString
// FullName:  StyleParser::GetWindowStyleExAsString
// Access:    public 
// Returns:   void
// Qualifier: const
// Parameter: const UINT uStyle_i
// Parameter: CString & csStyle_o
//************************************
void StyleParser::GetWindowStyleExAsString( const UINT uStyle_i, CString& csStyle_o ) const
{
    GetStylesAsFormattedString( uStyle_i,
                                csStyle_o,
                                g_WndExStyleMapper );
}

//************************************
// Method:    GetStyleStringForWindowByClassName
// FullName:  StyleParser::GetStyleStringForWindowByClassName
// Access:    public 
// Returns:   void
// Qualifier: const
// Parameter: const UINT uStyle_i
// Parameter: LPCTSTR lpctszClassName_i
// Parameter: CString & csStyle_o
//************************************
void StyleParser::GetStyleStringForWindowByClassName( const UINT uStyle_i, 
                                                      LPCTSTR lpctszClassName_i,
                                                      CString& csStyle_o ) const
{
    // Class name should be valid for further stuff
    if( !lpctszClassName_i )
    {
        return;
    }

    // Get style mapper for given class name
    const StyleMapper* pStyleMapper = FindStyleMapperForClass( lpctszClassName_i, m_ClassNameToStyleMapper );

    // Verify style mapper
    if( !pStyleMapper )
    {
        return;
    }

    // Get formatted styles
    GetStylesAsFormattedString( uStyle_i, 
                                csStyle_o, 
                                pStyleMapper );
}

//************************************
// Method:    GetStyleExStringForWindowByClassName
// FullName:  StyleParser::GetStyleExStringForWindowByClassName
// Access:    public 
// Returns:   void
// Qualifier: const
// Parameter: const UINT uStyleEx_i
// Parameter: LPCTSTR lpctszClassName_i
// Parameter: CString & csStyleEx_o
//************************************
void StyleParser::GetStyleExStringForWindowByClassName( const UINT uStyleEx_i,
                                                        LPCTSTR lpctszClassName_i,
                                                        CString& csStyleEx_o ) const
{
    // Now get control specific styles
    const StyleMapper* pStyleMapper = FindStyleMapperForClass( lpctszClassName_i, m_ClassNameToStyleExMapper );
    if( !pStyleMapper )
    {
        // We didn't find a style mapper hence return
        return;
    }

    GetStylesAsFormattedString( uStyleEx_i, csStyleEx_o, pStyleMapper );
}

//************************************
// Method:    FindStyleMapperForClass
// FullName:  StyleParser::FindStyleMapperForClass
// Access:    protected 
// Returns:   const StyleMapper*
// Qualifier: const
// Parameter: LPCTSTR lpctszClassName_i
// Parameter: const ClassNameToStyleMapper & ClassNameToStyleMapper_i
//************************************
const StyleMapper* StyleParser::FindStyleMapperForClass( LPCTSTR lpctszClassName_i, 
                                                         const ClassNameToStyleMapper& ClassNameToStyleMapper_i ) const
{
    const StyleMapper* pStyleMapper = 0;;
    ClassNameToStyleMapper_i.Lookup( lpctszClassName_i, pStyleMapper );
    return pStyleMapper;
}


//************************************
// Method:    GetStylesAsFormattedString
// FullName:  StyleParser::GetStylesAsFormattedString
// Access:    protected 
// Returns:   void
// Qualifier:
// Parameter: const UINT uStyle_i
// Parameter: CString & csStyle_o
// Parameter: const StyleMapper smStyleMapperObj_i[]
//************************************
void StyleParser::GetStylesAsFormattedString( UINT uStyle_i, 
                                              CString& csStyle_o, 
                                              const StyleMapper* psmStyleMapperObjArr_i) const
{
    LPCTSTR lpctszORSign = _T( " | " );

    // Append OR sign if string is not empty
    if( !csStyle_o.IsEmpty() )
    {
        csStyle_o += lpctszORSign;
    }

    // Loop through and get the styles as string
    while( psmStyleMapperObjArr_i->lpctszStyle )
    {
        if( Utils::IsValidMask( uStyle_i, psmStyleMapperObjArr_i->uStyle )) 
        {
            // Append style
            csStyle_o += psmStyleMapperObjArr_i->lpctszStyle;
            csStyle_o += lpctszORSign;

            // Remove style entry
            uStyle_i &= ~psmStyleMapperObjArr_i->uStyle;
        }// End if

        ++psmStyleMapperObjArr_i;
    }// End for

    // Remove last OR sign from string
    csStyle_o.TrimRight( lpctszORSign );
}// GetStylesAsFormattedString