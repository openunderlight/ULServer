# Microsoft Developer Studio Project File - Name="Net" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Net - Win32 Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Net.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Net.mak" CFG="Net - Win32 Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Net - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Net - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Net - Win32 Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "$/UlServer/libsrc/net"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Net - Win32 Release"

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

!ELSEIF  "$(CFG)" == "Net - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../MySQL/include" /I "../shared" /I "../rmsg" /I "../gmsg" /I "../smsg" /I "../dbi_mysql" /I "../server" /I "../util" /I "../private" /I "../net" /I "../db" /I "../dbi_gdbm" /I "../gdbm" /I "../../../pthreads-win32" /I "../pth" /I "/Projects/STL" /D "_LIB" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "UL_SERVER_WIN32" /D "USE_INLINE" /YX /FD /I /Projects/STL" /I /Projects/STL" /I /Projects/STL" /GZ " " " /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Net - Win32 Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Net___Win32_Unicode"
# PROP BASE Intermediate_Dir "Net___Win32_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Unicode"
# PROP Intermediate_Dir "Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../MySQL/include" /I "../shared" /I "../rmsg" /I "../gmsg" /I "../smsg" /I "../dbi_mysql" /I "../server" /I "../util" /I "../private" /I "../net" /I "../db" /I "../dbi_gdbm" /I "../gdbm" /I "../../../pthreads-win32" /I "../pth" /D "_LIB" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "UL_SERVER_WIN32" /D "USE_INLINE" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../MySQL/include" /I "../shared" /I "../rmsg" /I "../gmsg" /I "../smsg" /I "../dbi_mysql" /I "../server" /I "../util" /I "../private" /I "../net" /I "../db" /I "../dbi_gdbm" /I "../gdbm" /I "../../../pthreads-win32" /I "../pth" /I "/Projects/STL" /D "_LIB" /D "UL_SERVER_WIN32" /D "USE_INLINE" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_UNICODE" /D "UNICODE" /YX /FD /I /Projects/STL" /I /Projects/STL" /I /Projects/STL" /I /projects/stl" /GZ " " " " /c
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

# Name "Net - Win32 Release"
# Name "Net - Win32 Debug"
# Name "Net - Win32 Unicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\LmConnection.cpp
# End Source File
# Begin Source File

SOURCE=.\LmConnectionSet.cpp
# End Source File
# Begin Source File

SOURCE=.\LmSockAddr.cpp
# End Source File
# Begin Source File

SOURCE=.\LmSockAddrInet.cpp
# End Source File
# Begin Source File

SOURCE=.\LmSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\LmSockType.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\LmConnection.h
# End Source File
# Begin Source File

SOURCE=.\LmConnectionList.h
# End Source File
# Begin Source File

SOURCE=.\LmConnectionSet.h
# End Source File
# Begin Source File

SOURCE=.\LmSockAddr.h
# End Source File
# Begin Source File

SOURCE=.\LmSockAddrInet.h
# End Source File
# Begin Source File

SOURCE=.\LmSocket.h
# End Source File
# Begin Source File

SOURCE=.\LmSockType.h
# End Source File
# End Group
# Begin Group "Inline Files"

# PROP Default_Filter "*.i"
# Begin Source File

SOURCE=.\LmConnection.i
# End Source File
# Begin Source File

SOURCE=.\LmConnectionSet.i
# End Source File
# Begin Source File

SOURCE=.\LmSockAddr.i
# End Source File
# Begin Source File

SOURCE=.\LmSockAddrInet.i
# End Source File
# Begin Source File

SOURCE=.\LmSocket.i
# End Source File
# Begin Source File

SOURCE=.\LmSockType.i
# End Source File
# End Group
# End Target
# End Project
