# Microsoft Developer Studio Project File - Name="Boulder" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Boulder - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Boulder.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Boulder.mak" CFG="Boulder - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Boulder - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Boulder - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Boulder - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\WinRel"
# PROP BASE Intermediate_Dir ".\WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\WinRel"
# PROP Intermediate_Dir ".\WinRel"
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib /nologo /subsystem:windows /incremental:yes /machine:I386
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Boulder - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\WinDebug"
# PROP BASE Intermediate_Dir ".\WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\WinDebug"
# PROP Intermediate_Dir ".\WinDebug"
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Boulder - Win32 Release"
# Name "Boulder - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\BOULDER.C
# End Source File
# Begin Source File

SOURCE=.\BOULDER.DEF
# End Source File
# Begin Source File

SOURCE=.\Boulder.h
# End Source File
# Begin Source File

SOURCE=.\BOULDER.RC

!IF  "$(CFG)" == "Boulder - Win32 Release"

# ADD BASE RSC /l 0xc09
# ADD RSC /l 0xc09

!ELSEIF  "$(CFG)" == "Boulder - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\BOULDER.ICO
# End Source File
# Begin Source File

SOURCE=.\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\THINGS.BMP
# End Source File
# Begin Source File

SOURCE=.\toolbar1.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ALIEN.WAV
# End Source File
# Begin Source File

SOURCE=.\BOULDER.WAV
# End Source File
# Begin Source File

SOURCE=.\BUG.WAV
# End Source File
# Begin Source File

SOURCE=.\CHEESE.WAV
# End Source File
# Begin Source File

SOURCE=.\CLOCK.WAV
# End Source File
# Begin Source File

SOURCE=.\CRACK.WAV
# End Source File
# Begin Source File

SOURCE=.\DIAMOND.WAV
# End Source File
# Begin Source File

SOURCE=.\DIE.WAV
# End Source File
# Begin Source File

SOURCE=.\DIG.WAV
# End Source File
# Begin Source File

SOURCE=.\DOOR.WAV
# End Source File
# Begin Source File

SOURCE=.\EXIT.WAV
# End Source File
# Begin Source File

SOURCE=.\EXPLODE.WAV
# End Source File
# Begin Source File

SOURCE=.\MAGIC.WAV
# End Source File
# Begin Source File

SOURCE=.\MINE.WAV
# End Source File
# Begin Source File

SOURCE=.\MOVE.WAV
# End Source File
# Begin Source File

SOURCE=.\NUT.WAV
# End Source File
# Begin Source File

SOURCE=.\PICKUP.WAV
# End Source File
# Begin Source File

SOURCE=.\PUSH.WAV
# End Source File
# Begin Source File

SOURCE=.\SPLASH.WAV
# End Source File
# Begin Source File

SOURCE=.\SQUASH.WAV
# End Source File
# Begin Source File

SOURCE=.\TANK.WAV
# End Source File
# Begin Source File

SOURCE=.\WALL.WAV
# End Source File
# Begin Source File

SOURCE=.\WHEEL.WAV
# End Source File
# Begin Source File

SOURCE=.\YUM.WAV
# End Source File
# End Target
# End Project
