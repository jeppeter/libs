#ifndef _STYLE_PARSER_H_
#define _STYLE_PARSER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <AfxTempl.h>

struct StyleMapper
{
    UINT uStyle;
    LPCTSTR lpctszStyle;
};

#define MAKE_STYLE_MAPPER_ITEM( Style ) { Style, _T( #Style )}

static const int MAX_STYLE_ITEM_COUNT = 50;

extern StyleMapper g_ClassStyleMapper[];          // Class style
extern StyleMapper g_WndExStyleMapper[];          // Window ex style
extern StyleMapper g_WndStyleMapper[];            // Normal window style

extern StyleMapper g_DlgStyleMapper[];            // Dialog styles
extern StyleMapper g_EdtStyleMapper[];            // Edit control styles
extern StyleMapper g_ListCtrlStyleMapper[];       // List control styles
extern StyleMapper g_ListCtrlExStyleMapper[];     // List control ex styles
extern StyleMapper g_ListBoxStyleMapper[];        // List box styles
extern StyleMapper g_CommonCtrlStyleMapper[];     // Toolbar styles
extern StyleMapper g_ToolbarStyleMapper[];        // Toolbar styles
extern StyleMapper g_ToolbarExStyleMapper[];      // Toolbar ex styles
extern StyleMapper g_StatusBarStyleMapper[];      // Status bar styles
extern StyleMapper g_ButtonStyleMapper[];         // Button styles
extern StyleMapper g_StaticCtrlStyleMapper[];     // Static control styles
extern StyleMapper g_ComboCtrlStyleMapper[];      // Combo style
extern StyleMapper g_ComboExCtrlStyleExMapper[];  // ComboEx extended style
extern StyleMapper g_ScrollbarStyleMapper[];      // For Scrollbars
extern StyleMapper g_DateTimePickerStyleMapper[]; // For calendar/date time control
extern StyleMapper g_ProgressBarStyleMapper[];    // Progress bar styles
extern StyleMapper g_TreeCtrlStyleMapper[];       // Tree control
extern StyleMapper g_HeaderCtrlStyleMapper[];     // For header control
extern StyleMapper g_TabcontrolStyleMapper[];     // For header control
extern StyleMapper g_TabcontrolStyleExMapper[];   // Extended styles for tab control

class StyleParser  
{
    public:
	    StyleParser();
	    virtual ~StyleParser();

        void GetClassStyleAsString( const UINT uStyle_i, CString& csStyle_o ) const;
        void GetWindowStyleAsString( const UINT uStyle_i, CString& csStyle_o ) const;
        void GetWindowStyleExAsString( const UINT uStyle_i, CString& csStyle_o ) const;

        void GetStyleStringForWindowByClassName( const UINT uStyle_i, 
                                                 LPCTSTR lpctszClassName_i,
                                                 CString& csStyle_o ) const;

        void GetStyleExStringForWindowByClassName( const UINT uStyleEx_i,
                                                   LPCTSTR lpctszClassName_i,
                                                   CString& csStyleEx_o ) const;                                                

    protected:

        // Helper for sorting
        static int __cdecl CompareFunc( const void* pFirst, const void* pSecond );
        
        void SortStyleMappers();
        void SortStyleMapper( StyleMapper* pStyleMapper_i );
        DWORD GetStyleMapperItemCount( const StyleMapper* pStyleMapper_i );

        // Typedef for class name to style code mapper
        typedef CMap<CString, LPCTSTR, const StyleMapper*, const StyleMapper*&> ClassNameToStyleMapper;

        // Initializes class name map
        void InitClassNameMap();

        // Finds a style for a class
        const StyleMapper* FindStyleMapperForClass( LPCTSTR lpctszClassName_i, const ClassNameToStyleMapper& ClassNameToStyleMapper_i  ) const;

        // Helper for preparing a style string
        void GetStylesAsFormattedString( UINT uStyle_i, 
                                         CString& csStyle_o, 
                                         const StyleMapper* psmStyleMapperObj_i ) const;

        // Maintains a map of style mappers for a class name
        ClassNameToStyleMapper m_ClassNameToStyleMapper;
        ClassNameToStyleMapper m_ClassNameToStyleExMapper;
};// End StyleParser

#endif // _STYLE_PARSER_H_