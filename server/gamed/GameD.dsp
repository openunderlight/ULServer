# Microsoft Developer Studio Project File - Name="GameD" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=GameD - Win32 Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GameD.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GameD.mak" CFG="GameD - Win32 Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GameD - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "GameD - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "GameD - Win32 Unicode" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "$/UlServer/server/gamed"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GameD - Win32 Release"

# PROP BASE Use_MFC 0
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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../../../MySQL/include" /I "../../libsrc/shared" /I "../../libsrc/rmsg" /I "../../libsrc/gmsg" /I "../../libsrc/smsg" /I "../../libsrc/dbi_mysql" /I "../../libsrc/server" /I "../../libsrc/util" /I "../../libsrc/private" /I "../../libsrc/net" /I "../../libsrc/db" /I "../../libsrc/dbi_gdbm" /I "../../libsrc/gdbm" /I "../../../pthreads-win32" /I "../../libsrc/pth" /I "/Projects/STL" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "UL_SERVER_WIN32" /Yu"stdafx.h" /FD /I /Projects/STL" /I /Projects/STL" /I /Projects/STL" " " " /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ../../libsrc/db/release/db.lib ../../libsrc/dbi_mysql/release/dbi_mysql.lib ../../libsrc/gmsg/release/gmsg.lib ../../libsrc/net/release/net.lib ../../libsrc/private/release/private.lib ../../libsrc/pth/release/pth.lib ../../libsrc/rmsg/release/rmsg.lib ../../libsrc/server/release/server.lib ../../libsrc/shared/release/shared.lib ../../libsrc/smsg/release/smsg.lib ../../libsrc/util/release/util.lib ../../libsrc/gdbm/release/gdbm.lib ../../libsrc/dbi_gdbm/release/dbi_gdbm.lib ../../../MySQL/lib_release/LibMySQL.lib ../../../pthreads-win32/pthread.lib ws2_32.lib /nologo /subsystem:windows /machine:I386 /out:"\projects\UlServer\Prod\Bin\Release\GameD.exe"

!ELSEIF  "$(CFG)" == "GameD - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "GameD___Win32_Debug"
# PROP BASE Intermediate_Dir "GameD___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G6 /MTd /W3 /Gm /GX /ZI /Od /I "../../../MySQL/include" /I "../../libsrc/shared" /I "../../libsrc/rmsg" /I "../../libsrc/gmsg" /I "../../libsrc/smsg" /I "../../libsrc/dbi_mysql" /I "../../libsrc/server" /I "../../libsrc/util" /I "../../libsrc/private" /I "../../libsrc/net" /I "../../libsrc/db" /I "../../libsrc/dbi_gdbm" /I "../../libsrc/gdbm" /I "../../../pthreads-win32" /I "../../libsrc/pth" /I "/Projects/STL" /D "_WINDOWS" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "UL_SERVER_WIN32" /D "USE_INLINE" /D "CHINESE" /FR /FD /I /Projects/STL" /I /Projects/STL" /I /Projects/STL" /GZ " " " /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ../../libsrc/db/debug/db.lib ../../libsrc/dbi_mysql/debug/dbi_mysql.lib ../../libsrc/gmsg/debug/gmsg.lib ../../libsrc/net/debug/net.lib ../../libsrc/private/debug/private.lib ../../libsrc/pth/debug/pth.lib ../../libsrc/rmsg/debug/rmsg.lib ../../libsrc/server/debug/server.lib ../../libsrc/shared/debug/shared.lib ../../libsrc/smsg/debug/smsg.lib ../../libsrc/util/debug/util.lib ../../libsrc/gdbm/debug/gdbm.lib ../../libsrc/dbi_gdbm/debug/dbi_gdbm.lib ../../../MySQL/lib_debug/LibMySQL.lib ../../../pthreads-win32/pthread.lib ws2_32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"\projects\UlServer\Prod\Bin\Debug\GameD.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "GameD - Win32 Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "GameD___Win32_Unicode"
# PROP BASE Intermediate_Dir "GameD___Win32_Unicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Unicode"
# PROP Intermediate_Dir "Unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MTd /W3 /Gm /GX /ZI /Od /I "../../../MySQL/include" /I "../../libsrc/shared" /I "../../libsrc/rmsg" /I "../../libsrc/gmsg" /I "../../libsrc/smsg" /I "../../libsrc/dbi_mysql" /I "../../libsrc/server" /I "../../libsrc/util" /I "../../libsrc/private" /I "../../libsrc/net" /I "../../libsrc/db" /I "../../libsrc/dbi_gdbm" /I "../../libsrc/gdbm" /I "../../../pthreads-win32" /I "../../libsrc/pth" /D "_WINDOWS" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "UL_SERVER_WIN32" /D "USE_INLINE" /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /G6 /MTd /W3 /Gm /GX /ZI /Od /I "../../../MySQL/include" /I "../../libsrc/shared" /I "../../libsrc/rmsg" /I "../../libsrc/gmsg" /I "../../libsrc/smsg" /I "../../libsrc/dbi_mysql" /I "../../libsrc/server" /I "../../libsrc/util" /I "../../libsrc/private" /I "../../libsrc/net" /I "../../libsrc/db" /I "../../libsrc/dbi_gdbm" /I "../../libsrc/gdbm" /I "../../../pthreads-win32" /I "../../libsrc/pth" /I "/Projects/STL" /D "_WINDOWS" /D "UL_SERVER_WIN32" /D "USE_INLINE" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_UNICODE" /D "UNICODE" /FR /FD /I /Projects/STL" /I /Projects/STL" /I /Projects/STL" /I /projects/stl" /GZ " " " " /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ../../libsrc/db/debug/db.lib ../../libsrc/dbi_mysql/debug/dbi_mysql.lib ../../libsrc/gmsg/debug/gmsg.lib ../../libsrc/net/debug/net.lib ../../libsrc/private/debug/private.lib ../../libsrc/pth/debug/pth.lib ../../libsrc/rmsg/debug/rmsg.lib ../../libsrc/server/debug/server.lib ../../libsrc/shared/debug/shared.lib ../../libsrc/smsg/debug/smsg.lib ../../libsrc/util/debug/util.lib ../../libsrc/gdbm/debug/gdbm.lib ../../libsrc/dbi_gdbm/debug/dbi_gdbm.lib ../../../MySQL/lib_debug/LibMySQL.lib ../../../pthreads-win32/pthread.lib ws2_32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"\projects\UlServer\Prod\Bin\Debug\GameD.exe" /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ../../libsrc/db/debug/db.lib ../../libsrc/dbi_mysql/debug/dbi_mysql.lib ../../libsrc/gmsg/debug/gmsg.lib ../../libsrc/net/debug/net.lib ../../libsrc/private/debug/private.lib ../../libsrc/pth/debug/pth.lib ../../libsrc/rmsg/debug/rmsg.lib ../../libsrc/server/debug/server.lib ../../libsrc/shared/debug/shared.lib ../../libsrc/smsg/debug/smsg.lib ../../libsrc/util/debug/util.lib ../../libsrc/gdbm/debug/gdbm.lib ../../libsrc/dbi_gdbm/debug/dbi_gdbm.lib ../../../MySQL/lib_debug/LibMySQL.lib ../../../pthreads-win32/pthread.lib ws2_32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"\projects\UlServer\Prod\Bin\Debug\GameD.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "GameD - Win32 Release"
# Name "GameD - Win32 Debug"
# Name "GameD - Win32 Unicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\GsForwardThread.cpp
# End Source File
# Begin Source File

SOURCE=.\GsGameThread.cpp
# End Source File
# Begin Source File

SOURCE=.\GsInputDispatch.cpp
# End Source File
# Begin Source File

SOURCE=.\GsLevelSet.cpp
# End Source File
# Begin Source File

SOURCE=.\GsMain.cpp
# End Source File
# Begin Source File

SOURCE=.\GsMessageReader.cpp
# End Source File
# Begin Source File

SOURCE=.\GsNetworkInput.cpp
# End Source File
# Begin Source File

SOURCE=.\GsNetworkOutput.cpp
# End Source File
# Begin Source File

SOURCE=.\GsOutputDispatch.cpp
# End Source File
# Begin Source File

SOURCE=.\GsPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\GsPlayer1.cpp
# End Source File
# Begin Source File

SOURCE=.\GsPlayer2.cpp
# End Source File
# Begin Source File

SOURCE=.\GsPlayer3.cpp
# End Source File
# Begin Source File

SOURCE=.\GsPlayer4.cpp
# End Source File
# Begin Source File

SOURCE=.\GsPlayerSet.cpp
# End Source File
# Begin Source File

SOURCE=.\GsPlayerThread.cpp
# End Source File
# Begin Source File

SOURCE=.\GsPlayerThread1.cpp
# End Source File
# Begin Source File

SOURCE=.\GsPlayerThread2.cpp
# End Source File
# Begin Source File

SOURCE=.\GsPlayerThread3.cpp
# End Source File
# Begin Source File

SOURCE=.\GsPlayerThread4.cpp
# End Source File
# Begin Source File

SOURCE=.\GsPlayerThreadSet.cpp
# End Source File
# Begin Source File

SOURCE=.\GsPositionThread.cpp
# End Source File
# Begin Source File

SOURCE=.\GsSignalThread.cpp
# End Source File
# Begin Source File

SOURCE=.\GsUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\Main.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\GsForwardThread.h
# End Source File
# Begin Source File

SOURCE=.\GsGameThread.h
# End Source File
# Begin Source File

SOURCE=.\GsInputDispatch.h
# End Source File
# Begin Source File

SOURCE=.\GsLevelSet.h
# End Source File
# Begin Source File

SOURCE=.\GsMacros.h
# End Source File
# Begin Source File

SOURCE=.\GsMain.h
# End Source File
# Begin Source File

SOURCE=.\GsMessageReader.h
# End Source File
# Begin Source File

SOURCE=.\GsNetworkInput.h
# End Source File
# Begin Source File

SOURCE=.\GsNetworkOutput.h
# End Source File
# Begin Source File

SOURCE=.\GsOutputDispatch.h
# End Source File
# Begin Source File

SOURCE=.\GsPlayer.h
# End Source File
# Begin Source File

SOURCE=.\GsPlayerList.h
# End Source File
# Begin Source File

SOURCE=.\GsPlayerSet.h
# End Source File
# Begin Source File

SOURCE=.\GsPlayerThread.h
# End Source File
# Begin Source File

SOURCE=.\GsPlayerThreadSet.h
# End Source File
# Begin Source File

SOURCE=.\GsPositionThread.h
# End Source File
# Begin Source File

SOURCE=.\GsSignalThread.h
# End Source File
# Begin Source File

SOURCE=.\GsUtil.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Inline"

# PROP Default_Filter "*.i"
# Begin Source File

SOURCE=.\GsForwardThread.i
# End Source File
# Begin Source File

SOURCE=.\GsGameThread.i
# End Source File
# Begin Source File

SOURCE=.\GsInputDispatch.i
# End Source File
# Begin Source File

SOURCE=.\GsLevelSet.i
# End Source File
# Begin Source File

SOURCE=.\GsMain.i
# End Source File
# Begin Source File

SOURCE=.\GsMessageReader.i
# End Source File
# Begin Source File

SOURCE=.\GsNetworkInput.i
# End Source File
# Begin Source File

SOURCE=.\GsNetworkOutput.i
# End Source File
# Begin Source File

SOURCE=.\GsOutputDispatch.i
# End Source File
# Begin Source File

SOURCE=.\GsPlayer.i
# End Source File
# Begin Source File

SOURCE=.\GsPlayerSet.i
# End Source File
# Begin Source File

SOURCE=.\GsPlayerThread.i
# End Source File
# Begin Source File

SOURCE=.\GsPlayerThreadSet.i
# End Source File
# Begin Source File

SOURCE=.\GsPositionThread.i
# End Source File
# Begin Source File

SOURCE=.\GsSignalThread.i
# End Source File
# Begin Source File

SOURCE=.\GsUtil.i
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
