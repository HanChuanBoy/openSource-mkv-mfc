# Microsoft Developer Studio Project File - Name="kaxdemux" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=kaxdemux - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "kaxdemux.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "kaxdemux.mak" CFG="kaxdemux - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "kaxdemux - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "kaxdemux - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "kaxdemux - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /Gz /MT /W3 /GX /O2 /I "../../.." /I "../../../../../libebml/src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D nodebug=1 /D "STRICT" /YX /FD /LD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 strmbase.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x1d200000" /subsystem:windows /dll /machine:I386 /libpath:"../../../../lib"
# SUBTRACT LINK32 /nodefaultlib
# Begin Custom Build - Registering DirectX Media Filters...
TargetDir=.\Release
TargetPath=.\Release\kaxdemux.dll
InputPath=.\Release\kaxdemux.dll
SOURCE="$(InputPath)"

"$(TargetDir)\null.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32.exe /s /c "$(TargetPath)" 
	echo "pouette" > $(TargetDir)\null.txt 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "kaxdemux - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../.." /I "../../../../../libebml/src" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "DEBUG" /D "STRICT" /FR /YX /FD /GZ /LDd /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 strmbasd.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x1d200000" /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /libpath:"../../../../lib"
# SUBTRACT LINK32 /nodefaultlib
# Begin Custom Build - Registering DirectX Media Filters...
TargetDir=.\Debug
TargetPath=.\Debug\kaxdemux.dll
InputPath=.\Debug\kaxdemux.dll
SOURCE="$(InputPath)"

"$(TargetDir)\null.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32.exe /s /c "$(TargetPath)" 
	echo "pouette" > $(TargetDir)\null.txt 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "kaxdemux - Win32 Release"
# Name "kaxdemux - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\asyncio.cpp
# End Source File
# Begin Source File

SOURCE=.\kaxdemux.cpp
# End Source File
# Begin Source File

SOURCE=.\kaxdemux.DEF
# End Source File
# Begin Source File

SOURCE=.\kaxdemux.RC
# End Source File
# Begin Source File

SOURCE=.\kaxinpin.cpp
# End Source File
# Begin Source File

SOURCE=.\kaxoutpin.cpp
# End Source File
# Begin Source File

SOURCE=.\MatroskaReader.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\asyncio.h
# End Source File
# Begin Source File

SOURCE=.\codecs.h
# End Source File
# Begin Source File

SOURCE=.\global.h
# End Source File
# Begin Source File

SOURCE=.\kaxdemux.h
# End Source File
# Begin Source File

SOURCE=.\kaxinpin.h
# End Source File
# Begin Source File

SOURCE=.\kaxoutpin.h
# End Source File
# Begin Source File

SOURCE=.\MatroskaReader.h
# End Source File
# Begin Source File

SOURCE=.\OggDS.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
