# Microsoft Developer Studio Project File - Name="LevelD" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=LevelD - Win32 Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LevelD.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LevelD.mak" CFG="LevelD - Win32 Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LevelD - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "LevelD - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "LevelD - Win32 Unicode" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "$/UlServer/server/leveld"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LevelD - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../../../MySQL/include" /I "../../libsrc/shared" /I "../../libsrc/rmsg" /I "../../libsrc/gmsg" /I "../../libsrc/smsg" /I "../../libsrc/dbi_mysql" /I "../../libsrc/server" /I "../../libsrc/util" /I "../../libsrc/private" /I "../../libsrc/net" /I "../../libsrc/db" /I "../../libsrc/dbi_gdbm" /I "../../libsrc/gdbm" /I "../../../pthreads-win32" /I "../../libsrc/pth" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "UL_SERVER_WIN32" /FD /I /Projects/STL" /I /Projects/STL" /I /Projects/STL" /I /Projects/STL" " " " " /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ../../libsrc/db/release/db.lib ../../libsrc/dbi_mysql/release/dbi_mysql.lib ../../libsrc/gmsg/release/gmsg.lib ../../libsrc/net/release/net.lib ../../libsrc/private/release/private.lib ../../libsrc/pth/release/pth.lib ../../libsrc/rmsg/release/rmsg.lib ../../libsrc/server/release/server.lib ../../libsrc/shared/release/shared.lib ../../libsrc/smsg/release/smsg.lib ../../libsrc/util/release/util.lib ../../libsrc/gdbm/release/gdbm.lib ../../libsrc/dbi_gdbm/release/dbi_gdbm.lib ../../../MySQL/lib_release/LibMySQL.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ../../../pthreads-win32/pthread.lib ws2_32.lib winmm.lib /nologo /subsystem:windows /machine:I386 /out:"\projects\UlServer\Prod\Bin\Release\LevelD.exe"

!ELSEIF  "$(CFG)" == "LevelD - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LevelD___Win32_Debug"
# PROP BASE Intermediate_Dir "LevelD___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../MySQL/include" /I "../../libsrc/shared" /I "../../libsrc/rmsg" /I "../../libsrc/gmsg" /I "../../libsrc/smsg" /I "../../libsrc/dbi_mysql" /I "../../libsrc/server" /I "../../libsrc/util" /I "../../libsrc/private" /I "../../libsrc/net" /I "../../libsrc/db" /I "../../libsrc/dbi_gdbm" /I "../../libsrc/gdbm" /I "../../../pthreads-win32" /I "../../libsrc/pth" /D "_WINDOWS" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "UL_SERVER_WIN32" /D "USE_INLINE" /FR /FD /I /Projects/STL" /I /Projects/STL" /I /Projects/STL" /I /Projects/STL" /GZ " " " " /c
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
# ADD LINK32 ../../libsrc/db/debug/db.lib ../../libsrc/dbi_mysql/debug/dbi_mysql.lib ../../libsrc/gmsg/debug/gmsg.lib ../../libsrc/net/debug/net.lib ../../libsrc/private/debug/private.lib ../../libsrc/pth/debug/pth.lib ../../libsrc/rmsg/debug/rmsg.lib ../../libsrc/server/debug/server.lib ../../libsrc/shared/debug/shared.lib ../../libsrc/smsg/debug/smsg.lib ../../libsrc/util/debug/util.lib ../../libsrc/gdbm/debug/gdbm.lib ../../libsrc/dbi_gdbm/debug/dbi_gdbm.lib ../../../MySQL/lib_debug/LibMySQL.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ../../../pthreads-win32/pthread.lib ws2_32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /out:"\projects\UlServer\Prod\Bin\Debug\LevelD.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "LevelD - Win32 Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LevelD___Win32_Unicode"
# PROP BASE Intermediate_Dir "LevelD___Win32_Unicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Unicode"
# PROP Intermediate_Dir "Unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../MySQL/include" /I "../../libsrc/shared" /I "../../libsrc/rmsg" /I "../../libsrc/gmsg" /I "../../libsrc/smsg" /I "../../libsrc/dbi_mysql" /I "../../libsrc/server" /I "../../libsrc/util" /I "../../libsrc/private" /I "../../libsrc/net" /I "../../libsrc/db" /I "../../libsrc/dbi_gdbm" /I "../../libsrc/gdbm" /I "../../../pthreads-win32" /I "../../libsrc/pth" /D "_WINDOWS" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "UL_SERVER_WIN32" /D "USE_INLINE" /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../MySQL/include" /I "../../libsrc/shared" /I "../../libsrc/rmsg" /I "../../libsrc/gmsg" /I "../../libsrc/smsg" /I "../../libsrc/dbi_mysql" /I "../../libsrc/server" /I "../../libsrc/util" /I "../../libsrc/private" /I "../../libsrc/net" /I "../../libsrc/db" /I "../../libsrc/dbi_gdbm" /I "../../libsrc/gdbm" /I "../../../pthreads-win32" /I "../../libsrc/pth" /D "_WINDOWS" /D "UL_SERVER_WIN32" /D "USE_INLINE" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_UNICODE" /D "UNICODE" /FD /I /Projects/STL" /I /Projects/STL" /I /Projects/STL" /I /Projects/STL" /I /projects/stl" /GZ " " " " " /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ../../libsrc/db/debug/db.lib ../../libsrc/dbi_mysql/debug/dbi_mysql.lib ../../libsrc/gmsg/debug/gmsg.lib ../../libsrc/net/debug/net.lib ../../libsrc/private/debug/private.lib ../../libsrc/pth/debug/pth.lib ../../libsrc/rmsg/debug/rmsg.lib ../../libsrc/server/debug/server.lib ../../libsrc/shared/debug/shared.lib ../../libsrc/smsg/debug/smsg.lib ../../libsrc/util/debug/util.lib ../../libsrc/gdbm/debug/gdbm.lib ../../libsrc/dbi_gdbm/debug/dbi_gdbm.lib ../../../MySQL/lib_debug/LibMySQL.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ../../../pthreads-win32/pthread.lib ws2_32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /out:"\projects\UlServer\Prod\Bin\Debug\LevelD.exe" /pdbtype:sept
# ADD LINK32 ../../libsrc/db/debug/db.lib ../../libsrc/dbi_mysql/debug/dbi_mysql.lib ../../libsrc/gmsg/debug/gmsg.lib ../../libsrc/net/debug/net.lib ../../libsrc/private/debug/private.lib ../../libsrc/pth/debug/pth.lib ../../libsrc/rmsg/debug/rmsg.lib ../../libsrc/server/debug/server.lib ../../libsrc/shared/debug/shared.lib ../../libsrc/smsg/debug/smsg.lib ../../libsrc/util/debug/util.lib ../../libsrc/gdbm/debug/gdbm.lib ../../libsrc/dbi_gdbm/debug/dbi_gdbm.lib ../../../MySQL/lib_debug/LibMySQL.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ../../../pthreads-win32/pthread.lib ws2_32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /out:"\projects\UlServer\Prod\Bin\Debug\LevelD.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "LevelD - Win32 Release"
# Name "LevelD - Win32 Debug"
# Name "LevelD - Win32 Unicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\LsCodexText.cpp
# End Source File
# Begin Source File

SOURCE=.\LsComputeGroups.cpp
# End Source File
# Begin Source File

SOURCE=.\LsInputDispatch.cpp
# End Source File
# Begin Source File

SOURCE=.\LsItemGen.cpp
# End Source File
# Begin Source File

SOURCE=.\LsLevelSerials.cpp
# End Source File
# Begin Source File

SOURCE=.\LsLevelState.cpp
# End Source File
# Begin Source File

SOURCE=.\LsLevelThread.cpp
# End Source File
# Begin Source File

SOURCE=.\LsMain.cpp
# End Source File
# Begin Source File

SOURCE=.\LsMessageReader.cpp
# End Source File
# Begin Source File

SOURCE=.\LsNetworkInput.cpp
# End Source File
# Begin Source File

SOURCE=.\LsNetworkOutput.cpp
# End Source File
# Begin Source File

SOURCE=.\LsOutputDispatch.cpp
# End Source File
# Begin Source File

SOURCE=.\LsPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\LsPlayerSet.cpp
# End Source File
# Begin Source File

SOURCE=.\LsPositionThread.cpp
# End Source File
# Begin Source File

SOURCE=.\LsRoomState.cpp
# End Source File
# Begin Source File

SOURCE=.\LsRoomThread.cpp
# End Source File
# Begin Source File

SOURCE=.\LsRoomThread1.cpp
# End Source File
# Begin Source File

SOURCE=.\LsRoomThread2.cpp
# End Source File
# Begin Source File

SOURCE=.\LsRoomThread3.cpp
# End Source File
# Begin Source File

SOURCE=.\LsSignalThread.cpp
# End Source File
# Begin Source File

SOURCE=.\LsUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\Main.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\LsCodexText.h
# End Source File
# Begin Source File

SOURCE=.\LsComputeGroups.h
# End Source File
# Begin Source File

SOURCE=.\LsInputDispatch.h
# End Source File
# Begin Source File

SOURCE=.\LsItemGen.h
# End Source File
# Begin Source File

SOURCE=.\LsLevelSerials.h
# End Source File
# Begin Source File

SOURCE=.\LsLevelState.h
# End Source File
# Begin Source File

SOURCE=.\LsLevelThread.h
# End Source File
# Begin Source File

SOURCE=.\LsMacros.h
# End Source File
# Begin Source File

SOURCE=.\LsMain.h
# End Source File
# Begin Source File

SOURCE=.\LsMessageReader.h
# End Source File
# Begin Source File

SOURCE=.\LsNetworkInput.h
# End Source File
# Begin Source File

SOURCE=.\LsNetworkOutput.h
# End Source File
# Begin Source File

SOURCE=.\LsOutputDispatch.h
# End Source File
# Begin Source File

SOURCE=.\LsPlayer.h
# End Source File
# Begin Source File

SOURCE=.\LsPlayerList.h
# End Source File
# Begin Source File

SOURCE=.\LsPlayerSet.h
# End Source File
# Begin Source File

SOURCE=.\LsPositionThread.h
# End Source File
# Begin Source File

SOURCE=.\LsRoomState.h
# End Source File
# Begin Source File

SOURCE=.\LsRoomThread.h
# End Source File
# Begin Source File

SOURCE=.\LsSignalThread.h
# End Source File
# Begin Source File

SOURCE=.\LsUtil.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Inline Files"

# PROP Default_Filter "*.i"
# Begin Source File

SOURCE=.\LsCodexText.i
# End Source File
# Begin Source File

SOURCE=.\LsInputDispatch.i
# End Source File
# Begin Source File

SOURCE=.\LsItemGen.i
# End Source File
# Begin Source File

SOURCE=.\LsLevelSerials.i
# End Source File
# Begin Source File

SOURCE=.\LsLevelState.i
# End Source File
# Begin Source File

SOURCE=.\LsLevelThread.i
# End Source File
# Begin Source File

SOURCE=.\LsMain.i
# End Source File
# Begin Source File

SOURCE=.\LsMessageReader.i
# End Source File
# Begin Source File

SOURCE=.\LsNetworkInput.i
# End Source File
# Begin Source File

SOURCE=.\LsNetworkOutput.i
# End Source File
# Begin Source File

SOURCE=.\LsOutputDispatch.i
# End Source File
# Begin Source File

SOURCE=.\LsPlayer.i
# End Source File
# Begin Source File

SOURCE=.\LsPlayerSet.i
# End Source File
# Begin Source File

SOURCE=.\LsPositionThread.i
# End Source File
# Begin Source File

SOURCE=.\LsRoomState.i
# End Source File
# Begin Source File

SOURCE=.\LsRoomThread.i
# End Source File
# Begin Source File

SOURCE=.\LsSignalThread.i
# End Source File
# Begin Source File

SOURCE=.\LsUtil.i
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
