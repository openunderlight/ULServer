# Microsoft Developer Studio Project File - Name="Shared" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Shared - Win32 Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Shared.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Shared.mak" CFG="Shared - Win32 Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Shared - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Shared - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Shared - Win32 Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "$/UlServer/libsrc/shared"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Shared - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../../../MySQL/include" /I "../shared" /I "../rmsg" /I "../gmsg" /I "../smsg" /I "../dbi_mysql" /I "../server" /I "../util" /I "../private" /I "../net" /I "../db" /I "../dbi_gdbm" /I "../gdbm" /I "../../../pthreads-win32" /I "../pth" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "UL_SERVER_WIN32" /YX /FD /I /Projects/STL" /I /Projects/STL" /I /Projects/STL" /I /Projects/STL" " " " " /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Shared - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../MySQL/include" /I "../shared" /I "../rmsg" /I "../gmsg" /I "../smsg" /I "../dbi_mysql" /I "../server" /I "../util" /I "../private" /I "../net" /I "../db" /I "../dbi_gdbm" /I "../gdbm" /I "../../../pthreads-win32" /I "../pth" /D "_LIB" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "UL_SERVER_WIN32" /D "USE_INLINE" /FR /YX /FD /I /Projects/STL" /I /Projects/STL" /I /Projects/STL" /I /Projects/STL" /GZ " " " " /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Shared - Win32 Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Shared___Win32_Unicode"
# PROP BASE Intermediate_Dir "Shared___Win32_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Unicode"
# PROP Intermediate_Dir "Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../MySQL/include" /I "../shared" /I "../rmsg" /I "../gmsg" /I "../smsg" /I "../dbi_mysql" /I "../server" /I "../util" /I "../private" /I "../net" /I "../db" /I "../dbi_gdbm" /I "../gdbm" /I "../../../pthreads-win32" /I "../pth" /D "_LIB" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "UL_SERVER_WIN32" /D "USE_INLINE" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../MySQL/include" /I "../shared" /I "../rmsg" /I "../gmsg" /I "../smsg" /I "../dbi_mysql" /I "../server" /I "../util" /I "../private" /I "../net" /I "../db" /I "../dbi_gdbm" /I "../gdbm" /I "../../../pthreads-win32" /I "../pth" /D "_LIB" /D "UL_SERVER_WIN32" /D "USE_INLINE" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_UNICODE" /D "UNICODE" /YX /FD /I /Projects/STL" /I /Projects/STL" /I /Projects/STL" /I /Projects/STL" /I /projects/stl" /GZ " " " " " /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "Shared - Win32 Release"
# Name "Shared - Win32 Debug"
# Name "Shared - Win32 Unicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\LmArts.cpp
# End Source File
# Begin Source File

SOURCE=.\LmAvatar.cpp
# End Source File
# Begin Source File

SOURCE=.\LmBit32.cpp
# End Source File
# Begin Source File

SOURCE=.\LmItem.cpp
# End Source File
# Begin Source File

SOURCE=.\LmItemDefs.cpp
# End Source File
# Begin Source File

SOURCE=.\LmItemHdr.cpp
# End Source File
# Begin Source File

SOURCE=.\LmMesg.cpp
# End Source File
# Begin Source File

SOURCE=.\LmMesgBuf.cpp
# End Source File
# Begin Source File

SOURCE=.\LmMesgHdr.cpp
# End Source File
# Begin Source File

SOURCE=.\LmPeerUpdate.cpp
# End Source File
# Begin Source File

SOURCE=.\LmPosition.cpp
# End Source File
# Begin Source File

SOURCE=.\LmStats.cpp
# End Source File
# Begin Source File

SOURCE=.\LyraMessage.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\LmArts.h
# End Source File
# Begin Source File

SOURCE=.\LmAvatar.h
# End Source File
# Begin Source File

SOURCE=.\LmBit32.h
# End Source File
# Begin Source File

SOURCE=.\LmGame.h
# End Source File
# Begin Source File

SOURCE=.\LmItem.h
# End Source File
# Begin Source File

SOURCE=.\LmItemDefs.h
# End Source File
# Begin Source File

SOURCE=.\LmItemHdr.h
# End Source File
# Begin Source File

SOURCE=.\LmItemNames.h
# End Source File
# Begin Source File

SOURCE=.\LmMesg.h
# End Source File
# Begin Source File

SOURCE=.\LmMesgBuf.h
# End Source File
# Begin Source File

SOURCE=.\LmMesgHdr.h
# End Source File
# Begin Source File

SOURCE=.\LmMessage.h
# End Source File
# Begin Source File

SOURCE=.\LmPeerUpdate.h
# End Source File
# Begin Source File

SOURCE=.\LmPosition.h
# End Source File
# Begin Source File

SOURCE=.\LmStats.h
# End Source File
# Begin Source File

SOURCE=.\LmXPTable.h
# End Source File
# Begin Source File

SOURCE=.\LyraDefs.h
# End Source File
# Begin Source File

SOURCE=.\LyraMessage.h
# End Source File
# Begin Source File

SOURCE=.\SharedConstants.h
# End Source File
# Begin Source File

SOURCE=.\tchar.h
# End Source File
# Begin Source File

SOURCE=.\unix.h
# End Source File
# End Group
# Begin Group "Inline Files"

# PROP Default_Filter "*.i"
# Begin Source File

SOURCE=.\LmArts.i
# End Source File
# Begin Source File

SOURCE=.\LmAvatar.i
# End Source File
# Begin Source File

SOURCE=.\LmBit32.i
# End Source File
# Begin Source File

SOURCE=.\LmItem.i
# End Source File
# Begin Source File

SOURCE=.\LmItemDefs.i
# End Source File
# Begin Source File

SOURCE=.\LmItemHdr.i
# End Source File
# Begin Source File

SOURCE=.\LmMesg.i
# End Source File
# Begin Source File

SOURCE=.\LmMesgBuf.i
# End Source File
# Begin Source File

SOURCE=.\LmMesgHdr.i
# End Source File
# Begin Source File

SOURCE=.\LmPeerUpdate.i
# End Source File
# Begin Source File

SOURCE=.\LmPosition.i
# End Source File
# Begin Source File

SOURCE=.\LmStats.i
# End Source File
# End Group
# End Target
# End Project
