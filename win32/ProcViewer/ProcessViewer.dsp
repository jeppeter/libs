# Microsoft Developer Studio Project File - Name="ProcessViewer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ProcessViewer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ProcessViewer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ProcessViewer.mak" CFG="ProcessViewer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ProcessViewer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ProcessViewer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ProcessViewer - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /WX /GR /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "UNICODE" /D "_UNICODE" /D _WIN32_WINNT=0x0501 /D _WIN32_IE=0x0501 /D WINVER=0x0501 /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 uxtheme.lib uuid.lib psapi.lib version.lib shlwapi.lib msimg32.lib dbghelp.lib userenv.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /profile /debug /machine:I386
# SUBTRACT LINK32 /verbose /map /nodefaultlib
# Begin Special Build Tool
TargetPath=.\Release\ProcessViewer.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=copy                                                      "$(TargetPath)"                                                      .\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ProcessViewer - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /WX /Gm /GR /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "UNICODE" /D "_UNICODE" /D _WIN32_WINNT=0x0501 /D _WIN32_IE=0x0501 /D WINVER=0x0501 /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 uxtheme.lib version.lib userenv.lib dbghelp.lib psapi.lib msimg32.lib shlwapi.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /profile /debug /machine:I386
# SUBTRACT LINK32 /verbose /map /nodefaultlib

!ENDIF 

# Begin Target

# Name "ProcessViewer - Win32 Release"
# Name "ProcessViewer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\APIMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoStartupInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\BufferedDC.cpp
# End Source File
# Begin Source File

SOURCE=.\Color.cpp
# End Source File
# Begin Source File

SOURCE=.\DeferWindowPosMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\DividerWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\FileVersionInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\GeneralOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\HeaderCtrlEx.cpp
# End Source File
# Begin Source File

SOURCE=.\ListCtrlEx.cpp
# End Source File
# Begin Source File

SOURCE=.\Log.cpp
# End Source File
# Begin Source File

SOURCE=.\Module.cpp
# End Source File
# Begin Source File

SOURCE=.\ModuleCollection.cpp
# End Source File
# Begin Source File

SOURCE=.\ModuleDetailsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OSVer.cpp
# End Source File
# Begin Source File

SOURCE=.\Privilege.cpp
# End Source File
# Begin Source File

SOURCE=.\Process.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessCollection.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessDetailOptionsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessIOCounters.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessPrivilegeMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessSymbolCollection.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessTimes.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessViewer.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessViewer.rc
# End Source File
# Begin Source File

SOURCE=.\ProcessViewerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessWindowCollection.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgressBarEx.cpp
# End Source File
# Begin Source File

SOURCE=.\SearchDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StyleParser.cpp
# End Source File
# Begin Source File

SOURCE=.\SymbolCollection.cpp
# End Source File
# Begin Source File

SOURCE=.\TabPage.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeCtrlEx.cpp
# End Source File
# Begin Source File

SOURCE=.\Window.cpp
# End Source File
# Begin Source File

SOURCE=.\WindowCollection.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AggressiveOptimize.h
# End Source File
# Begin Source File

SOURCE=.\APIMgr.h
# End Source File
# Begin Source File

SOURCE=.\AutoStartupInfo.h
# End Source File
# Begin Source File

SOURCE=.\BufferedDC.h
# End Source File
# Begin Source File

SOURCE=.\Color.h
# End Source File
# Begin Source File

SOURCE=.\ColorConstants.h
# End Source File
# Begin Source File

SOURCE=.\DeferWindowPosMgr.h
# End Source File
# Begin Source File

SOURCE=.\DividerWnd.h
# End Source File
# Begin Source File

SOURCE=.\FileVersionInfo.h
# End Source File
# Begin Source File

SOURCE=.\GeneralOptions.h
# End Source File
# Begin Source File

SOURCE=.\HeaderCtrlEx.h
# End Source File
# Begin Source File

SOURCE=.\INodeData.h
# End Source File
# Begin Source File

SOURCE=.\ListCtrlEx.h
# End Source File
# Begin Source File

SOURCE=.\Log.h
# End Source File
# Begin Source File

SOURCE=.\Module.h
# End Source File
# Begin Source File

SOURCE=.\ModuleCollection.h
# End Source File
# Begin Source File

SOURCE=.\ModuleDetailsDlg.h
# End Source File
# Begin Source File

SOURCE=.\OptionsDlg.h
# End Source File
# Begin Source File

SOURCE=.\OSVer.h
# End Source File
# Begin Source File

SOURCE=.\Performance.h
# End Source File
# Begin Source File

SOURCE=.\Privilege.h
# End Source File
# Begin Source File

SOURCE=.\Process.h
# End Source File
# Begin Source File

SOURCE=.\ProcessCollection.h
# End Source File
# Begin Source File

SOURCE=.\ProcessDetailOptionsDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProcessIOCounters.h
# End Source File
# Begin Source File

SOURCE=.\ProcessPrivilegeMgr.h
# End Source File
# Begin Source File

SOURCE=.\ProcessSymbolCollection.h
# End Source File
# Begin Source File

SOURCE=.\ProcessTimes.h
# End Source File
# Begin Source File

SOURCE=.\ProcessViewer.h
# End Source File
# Begin Source File

SOURCE=.\ProcessViewerDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProcessWindowCollection.h
# End Source File
# Begin Source File

SOURCE=.\ProgressBarEx.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SearchDlg.h
# End Source File
# Begin Source File

SOURCE=.\Settings.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\StyleParser.h
# End Source File
# Begin Source File

SOURCE=.\SymbolCollection.h
# End Source File
# Begin Source File

SOURCE=.\TabPage.h
# End Source File
# Begin Source File

SOURCE=.\TreeCtrlEx.h
# End Source File
# Begin Source File

SOURCE=.\TypedefsInclude.h
# End Source File
# Begin Source File

SOURCE=.\Utils.h
# End Source File
# Begin Source File

SOURCE=.\Window.h
# End Source File
# Begin Source File

SOURCE=.\WindowCollection.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\clienticex.bmp
# End Source File
# Begin Source File

SOURCE=.\dnarrow.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ProcessViewer.rc2
# End Source File
# Begin Source File

SOURCE=.\res\ProcViewer.ico
# End Source File
# Begin Source File

SOURCE=.\uparrow.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\ProcessViewer.exe.manifest
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
