# Microsoft Developer Studio Project File - Name="GMsg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=GMsg - Win32 Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GMsg.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GMsg.mak" CFG="GMsg - Win32 Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GMsg - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "GMsg - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "GMsg - Win32 Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "$/UlServer/libsrc/gmsg"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GMsg - Win32 Release"

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

!ELSEIF  "$(CFG)" == "GMsg - Win32 Debug"

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

!ELSEIF  "$(CFG)" == "GMsg - Win32 Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "GMsg___Win32_Unicode"
# PROP BASE Intermediate_Dir "GMsg___Win32_Unicode"
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

# Name "GMsg - Win32 Release"
# Name "GMsg - Win32 Debug"
# Name "GMsg - Win32 Unicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\GMsg_AgentLogin.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_ChangeAvatar.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_ChangeStat.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_CreateItem.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_DestroyItem.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_DestroyRoomItem.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_Error.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_GetGoalHdrs.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_GetItem.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_GetItemDescription.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_GetLevelPlayers.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_GetPlayerName.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_GetReportHdrs.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_GiveItem.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_GiveItemAck.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_Goal.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_GotoLevel.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_GrantPPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_ItemDescription.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_ItemDrop.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_ItemPickup.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_LevelPlayers.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_LocateAvatar.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_LocateAvatarAck.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_LocateNewliesAck.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_Login.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_LoginAck.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_Logout.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_Ping.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_PostGoal.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_PostReport.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_PPointAck.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_PutItem.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_RcvGoalbookHdr.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_RcvGoalDetails.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_RcvGoalHdr.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_RcvGoalText.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_RcvPlayerName.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_RcvReportGoals.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_RcvReportHdr.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_RcvReportText.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_SenseDreamersAck.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_ServerDown.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_SetAvatarDescription.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_ShowItem.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_SummonAvatar.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_SummonAvatarAck.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_TakeItem.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_TakeItemAck.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_UpdateItem.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_UpdateStats.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_UsePPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\GMsg_ViewItem.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\GMsg.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_AgentLogin.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_All.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_ChangeAvatar.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_ChangeStat.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_CreateItem.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_DestroyItem.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_DestroyRoomItem.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_Error.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_GetGoalHdrs.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_GetItem.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_GetItemDescription.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_GetLevelPlayers.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_GetPlayerName.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_GetReportHdrs.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_GiveItem.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_GiveItemAck.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_Goal.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_GotoLevel.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_GrantPPoint.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_ItemDescription.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_ItemDrop.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_ItemPickup.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_LevelPlayers.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_LocateAvatar.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_LocateAvatarAck.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_LocateNewliesAck.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_Login.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_LoginAck.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_Logout.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_Ping.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_PostGoal.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_PostReport.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_PPointAck.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_PutItem.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_RcvGoalbookHdr.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_RcvGoalDetails.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_RcvGoalHdr.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_RcvGoalText.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_RcvPlayerName.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_RcvReportGoals.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_RcvReportHdr.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_RcvReportText.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_SenseDreamersAck.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_ServerDown.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_SetAvatarDescription.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_ShowItem.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_SummonAvatar.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_SummonAvatarAck.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_TakeItem.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_TakeItemAck.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_UpdateItem.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_UpdateStats.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_UsePPoint.h
# End Source File
# Begin Source File

SOURCE=.\GMsg_ViewItem.h
# End Source File
# End Group
# Begin Group "Inline Files"

# PROP Default_Filter "*.i"
# Begin Source File

SOURCE=.\GMsg_AgentLogin.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_ChangeAvatar.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_ChangeStat.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_CreateItem.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_DestroyItem.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_DestroyRoomItem.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_Error.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_GetGoalHdrs.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_GetItem.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_GetItemDescription.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_GetLevelPlayers.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_GetPlayerName.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_GetReportHdrs.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_GiveItem.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_GiveItemAck.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_Goal.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_GotoLevel.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_GrantPPoint.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_ItemDescription.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_ItemDrop.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_ItemPickup.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_LevelPlayers.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_LocateAvatar.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_LocateAvatarAck.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_LocateNewliesAck.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_Login.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_LoginAck.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_Logout.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_Ping.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_PostGoal.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_PostReport.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_PPointAck.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_PutItem.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_RcvGoalbookHdr.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_RcvGoalDetails.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_RcvGoalHdr.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_RcvGoalText.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_RcvPlayerName.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_RcvReportGoals.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_RcvReportHdr.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_RcvReportText.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_SenseDreamersAck.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_ServerDown.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_SetAvatarDescription.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_ShowItem.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_SummonAvatar.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_SummonAvatarAck.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_TakeItem.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_TakeItemAck.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_UpdateItem.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_UpdateStats.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_UsePPoint.i
# End Source File
# Begin Source File

SOURCE=.\GMsg_ViewItem.i
# End Source File
# End Group
# End Target
# End Project
