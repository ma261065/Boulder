# Microsoft Developer Studio Generated NMAKE File, Format Version 40001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Boulder - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Boulder - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Boulder - Win32 Release" && "$(CFG)" !=\
 "Boulder - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Boulder.mak" CFG="Boulder - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Boulder - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Boulder - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "Boulder - Win32 Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "Boulder - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : "$(OUTDIR)\Boulder.exe"

CLEAN : 
	-@erase ".\WinRel\Boulder.exe"
	-@erase ".\WinRel\BOULDER.OBJ"
	-@erase ".\WinRel\BOULDER.res"
	-@erase ".\WinRel\Boulder.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# SUBTRACT CPP /WX
CPP_PROJ=/nologo /ML /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/Boulder.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\WinRel/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
RSC_PROJ=/l 0xc09 /fo"$(INTDIR)/BOULDER.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Boulder.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib /nologo /subsystem:windows /incremental:yes /machine:I386
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib\
 /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)/Boulder.pdb"\
 /machine:I386 /def:".\BOULDER.DEF" /out:"$(OUTDIR)/Boulder.exe" 
DEF_FILE= \
	".\BOULDER.DEF"
LINK32_OBJS= \
	"$(INTDIR)/BOULDER.OBJ" \
	"$(INTDIR)/BOULDER.res"

"$(OUTDIR)\Boulder.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Boulder - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : "$(OUTDIR)\Boulder.exe"

CLEAN : 
	-@erase ".\WinDebug\vc40.pdb"
	-@erase ".\WinDebug\Boulder.exe"
	-@erase ".\WinDebug\BOULDER.OBJ"
	-@erase ".\WinDebug\BOULDER.res"
	-@erase ".\WinDebug\Boulder.ilk"
	-@erase ".\WinDebug\Boulder.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MLd /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/Boulder.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\WinDebug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
RSC_PROJ=/l 0xc09 /fo"$(INTDIR)/BOULDER.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Boulder.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib\
 uuid.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/Boulder.pdb" /debug /machine:I386 /def:".\BOULDER.DEF"\
 /out:"$(OUTDIR)/Boulder.exe" 
DEF_FILE= \
	".\BOULDER.DEF"
LINK32_OBJS= \
	"$(INTDIR)/BOULDER.OBJ" \
	"$(INTDIR)/BOULDER.res"

"$(OUTDIR)\Boulder.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "Boulder - Win32 Release"
# Name "Boulder - Win32 Debug"

!IF  "$(CFG)" == "Boulder - Win32 Release"

!ELSEIF  "$(CFG)" == "Boulder - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\BOULDER.C
DEP_CPP_BOULD=\
	".\Boulder.h"\
	
NODEP_CPP_BOULD=\
	".\..\..\..\source\c\boulder\resource.h"\
	

"$(INTDIR)\BOULDER.OBJ" : $(SOURCE) $(DEP_CPP_BOULD) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\BOULDER.RC
DEP_RSC_BOULDE=\
	".\BOULDER.ICO"\
	".\icon1.ico"\
	".\THINGS.BMP"\
	".\toolbar1.bmp"\
	".\WHEEL.WAV"\
	".\SPLASH.WAV"\
	".\YUM.WAV"\
	".\ALIEN.WAV"\
	".\TANK.WAV"\
	".\NUT.WAV"\
	".\CRACK.WAV"\
	".\DOOR.WAV"\
	".\MAGIC.WAV"\
	".\WALL.WAV"\
	".\BUG.WAV"\
	".\CHEESE.WAV"\
	".\EXIT.WAV"\
	".\MOVE.WAV"\
	".\EXPLODE.WAV"\
	".\DIE.WAV"\
	".\PUSH.WAV"\
	".\DIAMOND.WAV"\
	".\PICKUP.WAV"\
	".\BOULDER.WAV"\
	".\DIG.WAV"\
	".\CLOCK.WAV"\
	".\MINE.WAV"\
	".\SQUASH.WAV"\
	".\cursor1.cur"\
	".\Boulder.h"\
	

!IF  "$(CFG)" == "Boulder - Win32 Release"

# ADD BASE RSC /l 0xc09
# ADD RSC /l 0xc09

"$(INTDIR)\BOULDER.res" : $(SOURCE) $(DEP_RSC_BOULDE) "$(INTDIR)"
   $(RSC) /l 0xc09 /fo"$(INTDIR)/BOULDER.res" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "Boulder - Win32 Debug"


"$(INTDIR)\BOULDER.res" : $(SOURCE) $(DEP_RSC_BOULDE) "$(INTDIR)"
   $(RSC) /l 0xc09 /fo"$(INTDIR)/BOULDER.res" /d "_DEBUG" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\BOULDER.DEF

!IF  "$(CFG)" == "Boulder - Win32 Release"

!ELSEIF  "$(CFG)" == "Boulder - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Boulder.h

!IF  "$(CFG)" == "Boulder - Win32 Release"

!ELSEIF  "$(CFG)" == "Boulder - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\resource.h

!IF  "$(CFG)" == "Boulder - Win32 Release"

!ELSEIF  "$(CFG)" == "Boulder - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
