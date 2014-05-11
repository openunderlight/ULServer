# Microsoft Developer Studio Project File - Name="SMsg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=SMsg - Win32 Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SMsg.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SMsg.mak" CFG="SMsg - Win32 Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SMsg - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "SMsg - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "SMsg - Win32 Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "$/UlServer/libsrc/smsg"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SMsg - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../../../MySQL/include" /I "../shared" /I "../rmsg" /I "../gmsg" /I "../smsg" /I "../dbi_mysql" /I "../server" /I "../util" /I "../private" /I "../net" /I "../db" /I "../dbi_gdbm" /I "../gdbm" /I "../../../pthreads-win32" /I "../pth" /D "NDEBUG" /D "_LIB" /D "WIN32" /D "_MBCS" /D "UL_SERVER_WIN32" /YX /FD /I /Projects/STL" /I /Projects/STL" /I /Projects/STL" /I /Projects/STL" " " " " /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "SMsg - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../MySQL/include" /I "../shared" /I "../rmsg" /I "../gmsg" /I "../smsg" /I "../dbi_mysql" /I "../server" /I "../util" /I "../private" /I "../net" /I "../db" /I "../dbi_gdbm" /I "../gdbm" /I "../../../pthreads-win32" /I "../pth" /D "_LIB" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "UL_SERVER_WIN32" /D "USE_INLINE" /YX /FD /I /Projects/STL" /I /Projects/STL" /I /Projects/STL" /I /Projects/STL" /GZ " " " " /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "SMsg - Win32 Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SMsg___Win32_Unicode"
# PROP BASE Intermediate_Dir "SMsg___Win32_Unicode"
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

# Name "SMsg - Win32 Release"
# Name "SMsg - Win32 Debug"
# Name "SMsg - Win32 Unicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\SMsg_ConnStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_DBQuery.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_DBQueryAck.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_DestroyRoomItem.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_DS_Action.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_DumpState.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_Error.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_GetItem.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_GetLevelPlayers.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_GetServerStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_GiveItem.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_GiveItemAck.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_GS_Action.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_GS_Login.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_GS_Logout.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_ItemDrop.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_ItemPickup.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_LevelLogin.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_LocateAvatar.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_Login.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_Logout.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_LS_Action.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_LS_Login.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_PartyLeader.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_Ping.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_PlayerStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_Proxy.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_PutItem.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_RecvMesg.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_ResetPort.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_RotateLogs.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_SendMesg.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_ServerStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_SetAvatarDescription.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_ShowItem.cpp
# End Source File
# Begin Source File

SOURCE=.\SMsg_TakeItemAck.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\SMsg.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_All.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_ConnStatus.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_DBQuery.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_DBQueryAck.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_DestroyRoomItem.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_DS_Action.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_DumpState.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_Error.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_GetItem.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_GetLevelPlayers.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_GetServerStatus.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_GiveItem.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_GiveItemAck.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_GS_Action.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_GS_Login.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_GS_Logout.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_ItemDrop.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_ItemPickup.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_LevelLogin.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_LocateAvatar.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_Login.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_Logout.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_LS_Action.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_LS_Login.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_PartyLeader.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_Ping.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_PlayerStatus.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_Proxy.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_PutItem.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_RecvMesg.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_ResetPort.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_RotateLogs.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_SendMesg.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_ServerStatus.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_SetAvatarDescription.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_ShowItem.h
# End Source File
# Begin Source File

SOURCE=.\SMsg_TakeItemAck.h
# End Source File
# End Group
# Begin Group "Inline Files"

# PROP Default_Filter "*.i"
# Begin Source File

SOURCE=.\SMsg_ConnStatus.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_DBQuery.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_DBQueryAck.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_DestroyRoomItem.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_DS_Action.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_DumpState.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_Error.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_GetItem.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_GetLevelPlayers.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_GetServerStatus.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_GiveItem.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_GiveItemAck.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_GS_Action.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_GS_Login.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_GS_Logout.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_ItemDrop.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_ItemPickup.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_LevelLogin.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_LocateAvatar.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_Login.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_Logout.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_LS_Action.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_LS_Login.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_PartyLeader.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_Ping.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_PlayerStatus.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_Proxy.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_PutItem.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_RecvMesg.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_ResetPort.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_RotateLogs.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_SendMesg.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_ServerStatus.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_SetAvatarDescription.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_ShowItem.i
# End Source File
# Begin Source File

SOURCE=.\SMsg_TakeItemAck.i
# End Source File
# End Group
# End Target
# End Project
