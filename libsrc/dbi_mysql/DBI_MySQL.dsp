# Microsoft Developer Studio Project File - Name="DBI_MySQL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=DBI_MySQL - Win32 Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DBI_MySQL.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DBI_MySQL.mak" CFG="DBI_MySQL - Win32 Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DBI_MySQL - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "DBI_MySQL - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "DBI_MySQL - Win32 Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "$/UlServer/libsrc/dbi_mysql"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DBI_MySQL - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../../../MySQL/include" /I "../shared" /I "../rmsg" /I "../gmsg" /I "../smsg" /I "../dbi_mysql" /I "../server" /I "../util" /I "../private" /I "../net" /I "../db" /I "../dbi_gdbm" /I "../gdbm" /I "../../../pthreads-win32" /I "../pth" /I "/Projects/STL" /D "NDEBUG" /D "_LIB" /D "WIN32" /D "_MBCS" /D "UL_SERVER_WIN32" /YX /FD /I /Projects/STL" /I /Projects/STL" /I /Projects/STL" " " " /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "DBI_MySQL - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../MySQL/include" /I "../shared" /I "../rmsg" /I "../gmsg" /I "../smsg" /I "../dbi_mysql" /I "../server" /I "../util" /I "../private" /I "../net" /I "../db" /I "../dbi_gdbm" /I "../gdbm" /I "../../../pthreads-win32" /I "../pth" /I "/Projects/STL" /D "_LIB" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "UL_SERVER_WIN32" /D "USE_INLINE" /FR /YX /FD /I /Projects/STL" /I /Projects/STL" /I /Projects/STL" /GZ " " " /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "DBI_MySQL - Win32 Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DBI_MySQL___Win32_Unicode"
# PROP BASE Intermediate_Dir "DBI_MySQL___Win32_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Unicode"
# PROP Intermediate_Dir "Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../MySQL/include" /I "../shared" /I "../rmsg" /I "../gmsg" /I "../smsg" /I "../dbi_mysql" /I "../server" /I "../util" /I "../private" /I "../net" /I "../db" /I "../dbi_gdbm" /I "../gdbm" /I "../../../pthreads-win32" /I "../pth" /D "_LIB" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "UL_SERVER_WIN32" /D "USE_INLINE" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../MySQL/include" /I "../shared" /I "../rmsg" /I "../gmsg" /I "../smsg" /I "../dbi_mysql" /I "../server" /I "../util" /I "../private" /I "../net" /I "../db" /I "../dbi_gdbm" /I "../gdbm" /I "../../../pthreads-win32" /I "../pth" /I "/Projects/STL" /D "_LIB" /D "UL_SERVER_WIN32" /D "USE_INLINE" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_UNICODE" /D "UNICODE" /YX /FD /I /Projects/STL" /I /Projects/STL" /I /Projects/STL" /GZ " " " /c
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

# Name "DBI_MySQL - Win32 Release"
# Name "DBI_MySQL - Win32 Debug"
# Name "DBI_MySQL - Win32 Unicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\LmBillingDBC.cpp
# End Source File
# Begin Source File

SOURCE=.\LmGuildDBC.cpp
# End Source File
# Begin Source File

SOURCE=.\LmItemDBC.cpp
# End Source File
# Begin Source File

SOURCE=.\LmLevelDBC.cpp
# End Source File
# Begin Source File

SOURCE=.\LmPlayerDBC.cpp
# End Source File
# Begin Source File

SOURCE=.\LmServerDBC.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\LmBillingDBC.h
# End Source File
# Begin Source File

SOURCE=.\LmGuildDBC.h
# End Source File
# Begin Source File

SOURCE=.\LmItemDBC.h
# End Source File
# Begin Source File

SOURCE=.\LmLevelDBC.h
# End Source File
# Begin Source File

SOURCE=.\LmPlayerDBC.h
# End Source File
# Begin Source File

SOURCE=.\LmServerDBC.h
# End Source File
# End Group
# Begin Group "Inline Files"

# PROP Default_Filter "*.i"
# Begin Source File

SOURCE=.\LmBillingDBC.i
# End Source File
# Begin Source File

SOURCE=.\LmGuildDBC.i
# End Source File
# Begin Source File

SOURCE=.\LmItemDBC.i
# End Source File
# Begin Source File

SOURCE=.\LmLevelDBC.i
# End Source File
# Begin Source File

SOURCE=.\LmPlayerDBC.i
# End Source File
# Begin Source File

SOURCE=.\LmServerDBC.i
# End Source File
# End Group
# End Target
# End Project
