# Microsoft Developer Studio Project File - Name="MasterD" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MasterD - Win32 Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MasterD.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MasterD.mak" CFG="MasterD - Win32 Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MasterD - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "MasterD - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MasterD - Win32 Unicode" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MasterD - Win32 Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../MySQL/include" /I "../../libsrc/shared" /I "../../libsrc/rmsg" /I "../../libsrc/gmsg" /I "../../libsrc/smsg" /I "../../libsrc/dbi_mysql" /I "../../libsrc/server" /I "../../libsrc/util" /I "../../libsrc/private" /I "../../libsrc/net" /I "../../libsrc/db" /I "../../libsrc/dbi_gdbm" /I "../../libsrc/gdbm" /I "../../../pthreads-win32" /I "../../libsrc/pth" /D "_WINDOWS" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "UL_SERVER_WIN32" /D "USE_INLINE" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ../../libsrc/db/debug/db.lib ../../libsrc/dbi_mysql/debug/dbi_mysql.lib ../../libsrc/gmsg/debug/gmsg.lib ../../libsrc/net/debug/net.lib ../../libsrc/private/debug/private.lib ../../libsrc/pth/debug/pth.lib ../../libsrc/rmsg/debug/rmsg.lib ../../libsrc/server/debug/server.lib ../../libsrc/shared/debug/shared.lib ../../libsrc/smsg/debug/smsg.lib ../../libsrc/util/debug/util.lib ../../../MySQL/lib_debug/LibMySQL.lib ../../../pthreads-win32/pthread.lib ws2_32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"\projects\UlServer\Prod\Bin\Debug\MasterD.exe" /pdbtype:sept
# Begin Custom Build - Performing registration
OutDir=.\Debug
TargetPath=\projects\UlServer\Prod\Bin\Debug\MasterD.exe
InputPath=\projects\UlServer\Prod\Bin\Debug\MasterD.exe
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(TargetPath)" /RegServer 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	echo Server registration done! 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "MasterD - Win32 Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MasterD___Win32_Release"
# PROP BASE Intermediate_Dir "MasterD___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O1 /I "../../../MySQL/include" /I "../../libsrc/shared" /I "../../libsrc/rmsg" /I "../../libsrc/gmsg" /I "../../libsrc/smsg" /I "../../libsrc/dbi_mysql" /I "../../libsrc/server" /I "../../libsrc/util" /I "../../libsrc/private" /I "../../libsrc/net" /I "../../libsrc/db" /I "../../libsrc/dbi_gdbm" /I "../../libsrc/gdbm" /I "../../../pthreads-win32" /I "../../libsrc/pth" /D "NDEBUG" /D "_MBCS" /D "_ATL_DLL" /D "WIN32" /D "_WINDOWS" /D "UL_SERVER_WIN32" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O1 /I "../../../MySQL/include" /I "../../libsrc/shared" /I "../../libsrc/rmsg" /I "../../libsrc/gmsg" /I "../../libsrc/smsg" /I "../../libsrc/dbi_mysql" /I "../../libsrc/server" /I "../../libsrc/util" /I "../../libsrc/private" /I "../../libsrc/net" /I "../../libsrc/db" /I "../../libsrc/dbi_gdbm" /I "../../libsrc/gdbm" /I "../../../pthreads-win32" /I "../../libsrc/pth" /D "NDEBUG" /D "_MBCS" /D "_ATL_DLL" /D "WIN32" /D "_WINDOWS" /D "UL_SERVER_WIN32" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ../../libsrc/db/release/db.lib ../../libsrc/dbi_mysql/release/dbi_mysql.lib ../../libsrc/gmsg/release/gmsg.lib ../../libsrc/net/release/net.lib ../../libsrc/private/release/private.lib ../../libsrc/pth/release/pth.lib ../../libsrc/rmsg/release/rmsg.lib ../../libsrc/server/release/server.lib ../../libsrc/shared/release/shared.lib ../../libsrc/smsg/release/smsg.lib ../../libsrc/util/release/util.lib ../../../MySQL/lib_release/LibMySQL.lib ../../../pthreads-win32/pthread.lib ws2_32.lib /nologo /subsystem:windows /machine:I386 /out:"\projects\UlServer\Prod\Bin\Release\MasterD.exe"
# Begin Custom Build - Performing registration
OutDir=.\Release
TargetPath=\projects\UlServer\Prod\Bin\Release\MasterD.exe
InputPath=\projects\UlServer\Prod\Bin\Release\MasterD.exe
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(TargetPath)" /RegServer 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	echo Server registration done! 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "MasterD - Win32 Unicode"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MasterD___Win32_Unicode"
# PROP BASE Intermediate_Dir "MasterD___Win32_Unicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Unicode"
# PROP Intermediate_Dir "Unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../MySQL/include" /I "../../libsrc/shared" /I "../../libsrc/rmsg" /I "../../libsrc/gmsg" /I "../../libsrc/smsg" /I "../../libsrc/dbi_mysql" /I "../../libsrc/server" /I "../../libsrc/util" /I "../../libsrc/private" /I "../../libsrc/net" /I "../../libsrc/db" /I "../../libsrc/dbi_gdbm" /I "../../libsrc/gdbm" /I "../../../pthreads-win32" /I "../../libsrc/pth" /D "_WINDOWS" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "UL_SERVER_WIN32" /D "USE_INLINE" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../MySQL/include" /I "../../libsrc/shared" /I "../../libsrc/rmsg" /I "../../libsrc/gmsg" /I "../../libsrc/smsg" /I "../../libsrc/dbi_mysql" /I "../../libsrc/server" /I "../../libsrc/util" /I "../../libsrc/private" /I "../../libsrc/net" /I "../../libsrc/db" /I "../../libsrc/dbi_gdbm" /I "../../libsrc/gdbm" /I "../../../pthreads-win32" /I "../../libsrc/pth" /D "_WINDOWS" /D "UL_SERVER_WIN32" /D "USE_INLINE" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_UNICODE" /D "UNICODE" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ../../libsrc/db/debug/db.lib ../../libsrc/dbi_mysql/debug/dbi_mysql.lib ../../libsrc/gmsg/debug/gmsg.lib ../../libsrc/net/debug/net.lib ../../libsrc/private/debug/private.lib ../../libsrc/pth/debug/pth.lib ../../libsrc/rmsg/debug/rmsg.lib ../../libsrc/server/debug/server.lib ../../libsrc/shared/debug/shared.lib ../../libsrc/smsg/debug/smsg.lib ../../libsrc/util/debug/util.lib ../../../MySQL/lib_debug/LibMySQL.lib ../../../pthreads-win32/pthread.lib ws2_32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"\projects\UlServer\Prod\Bin\Debug\MasterD.exe" /pdbtype:sept
# ADD LINK32 ../../libsrc/db/debug/db.lib ../../libsrc/dbi_mysql/debug/dbi_mysql.lib ../../libsrc/gmsg/debug/gmsg.lib ../../libsrc/net/debug/net.lib ../../libsrc/private/debug/private.lib ../../libsrc/pth/debug/pth.lib ../../libsrc/rmsg/debug/rmsg.lib ../../libsrc/server/debug/server.lib ../../libsrc/shared/debug/shared.lib ../../libsrc/smsg/debug/smsg.lib ../../libsrc/util/debug/util.lib ../../../MySQL/lib_debug/LibMySQL.lib ../../../pthreads-win32/pthread.lib ws2_32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"\projects\UlServer\Prod\Bin\Debug\MasterD.exe" /pdbtype:sept
# Begin Custom Build - Performing registration
OutDir=.\Unicode
TargetPath=\projects\UlServer\Prod\Bin\Debug\MasterD.exe
InputPath=\projects\UlServer\Prod\Bin\Debug\MasterD.exe
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(TargetPath)" /RegServer 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	echo Server registration done! 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "MasterD - Win32 Debug"
# Name "MasterD - Win32 Release"
# Name "MasterD - Win32 Unicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\MasterD.cpp
# End Source File
# Begin Source File

SOURCE=.\MasterD.idl
# ADD MTL /tlb ".\MasterD.tlb" /h "MasterD.h" /iid "MasterD_i.c" /Oicf
# End Source File
# Begin Source File

SOURCE=.\MasterD.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\MasterD.rgs
# End Source File
# End Group
# End Target
# End Project
