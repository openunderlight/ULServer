# Microsoft Developer Studio Project File - Name="rmsg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=rmsg - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "rmsg.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "rmsg.mak" CFG="rmsg - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "rmsg - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "rmsg - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "$/UlServer/libsrc/rmsg"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "rmsg - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "rmsg - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
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

# Name "rmsg - Win32 Release"
# Name "rmsg - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\RmRemotePlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\RMsg_ChangeAvatar.cpp
# End Source File
# Begin Source File

SOURCE=.\RMsg_CupSummons.cpp
# End Source File
# Begin Source File

SOURCE=.\RMsg_EnterRoom.cpp
# End Source File
# Begin Source File

SOURCE=.\RMsg_Error.cpp
# End Source File
# Begin Source File

SOURCE=.\RMsg_GetAvatarDescription.cpp
# End Source File
# Begin Source File

SOURCE=.\RMsg_GotoRoom.cpp
# End Source File
# Begin Source File

SOURCE=.\RMsg_ItemDrop.cpp
# End Source File
# Begin Source File

SOURCE=.\RMsg_ItemHdrDrop.cpp
# End Source File
# Begin Source File

SOURCE=.\RMsg_ItemPickup.cpp
# End Source File
# Begin Source File

SOURCE=.\RMsg_JoinedParty.cpp
# End Source File
# Begin Source File

SOURCE=.\RMsg_LeaveRoom.cpp
# End Source File
# Begin Source File

SOURCE=.\RMsg_LoginAck.cpp
# End Source File
# Begin Source File

SOURCE=.\RMsg_Logout.cpp
# End Source File
# Begin Source File

SOURCE=.\RMsg_NewlyAwakened.cpp
# End Source File
# Begin Source File

SOURCE=.\RMsg_Party.cpp
# End Source File
# Begin Source File

SOURCE=.\RMsg_PartyInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\RMsg_Ping.cpp
# End Source File
# Begin Source File

SOURCE=.\RMsg_PlayerMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\RMsg_PlayerUpdate.cpp
# End Source File
# Begin Source File

SOURCE=.\RMsg_RcvAvatarDescription.cpp
# End Source File
# Begin Source File

SOURCE=.\RMsg_RoomLoginAck.cpp
# End Source File
# Begin Source File

SOURCE=.\RMsg_Speech.cpp
# End Source File
# Begin Source File

SOURCE=.\RMsg_Update.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\LmStats.h
# End Source File
# Begin Source File

SOURCE=.\RmRemotePlayer.h
# End Source File
# Begin Source File

SOURCE=.\RMsg.h
# End Source File
# Begin Source File

SOURCE=.\RMsg_All.h
# End Source File
# Begin Source File

SOURCE=.\RMsg_ChangeAvatar.h
# End Source File
# Begin Source File

SOURCE=.\RMsg_CupSummons.h
# End Source File
# Begin Source File

SOURCE=.\RMsg_EnterRoom.h
# End Source File
# Begin Source File

SOURCE=.\RMsg_Error.h
# End Source File
# Begin Source File

SOURCE=.\RMsg_GetAvatarDescription.h
# End Source File
# Begin Source File

SOURCE=.\RMsg_GotoRoom.h
# End Source File
# Begin Source File

SOURCE=.\RMsg_ItemDrop.h
# End Source File
# Begin Source File

SOURCE=.\RMsg_ItemHdrDrop.h
# End Source File
# Begin Source File

SOURCE=.\RMsg_ItemPickup.h
# End Source File
# Begin Source File

SOURCE=.\RMsg_JoinedParty.h
# End Source File
# Begin Source File

SOURCE=.\RMsg_LeaveRoom.h
# End Source File
# Begin Source File

SOURCE=.\RMsg_LoginAck.h
# End Source File
# Begin Source File

SOURCE=.\RMsg_Logout.h
# End Source File
# Begin Source File

SOURCE=.\RMsg_NewlyAwakened.h
# End Source File
# Begin Source File

SOURCE=.\RMsg_Party.h
# End Source File
# Begin Source File

SOURCE=.\RMsg_PartyInfo.h
# End Source File
# Begin Source File

SOURCE=.\RMsg_Ping.h
# End Source File
# Begin Source File

SOURCE=.\RMsg_PlayerMsg.h
# End Source File
# Begin Source File

SOURCE=.\RMsg_PlayerUpdate.h
# End Source File
# Begin Source File

SOURCE=.\RMsg_RcvAvatarDescription.h
# End Source File
# Begin Source File

SOURCE=.\RMsg_RoomLoginAck.h
# End Source File
# Begin Source File

SOURCE=.\RMsg_Speech.h
# End Source File
# Begin Source File

SOURCE=.\RMsg_Update.h
# End Source File
# End Group
# Begin Group "Inline Files"

# PROP Default_Filter "*.i"
# Begin Source File

SOURCE=.\RmRemotePlayer.i
# End Source File
# Begin Source File

SOURCE=.\RMsg_ChangeAvatar.i
# End Source File
# Begin Source File

SOURCE=.\RMsg_CupSummons.i
# End Source File
# Begin Source File

SOURCE=.\RMsg_EnterRoom.i
# End Source File
# Begin Source File

SOURCE=.\RMsg_Error.i
# End Source File
# Begin Source File

SOURCE=.\RMsg_GetAvatarDescription.i
# End Source File
# Begin Source File

SOURCE=.\RMsg_GotoRoom.i
# End Source File
# Begin Source File

SOURCE=.\RMsg_ItemDrop.i
# End Source File
# Begin Source File

SOURCE=.\RMsg_ItemHdrDrop.i
# End Source File
# Begin Source File

SOURCE=.\RMsg_ItemPickup.i
# End Source File
# Begin Source File

SOURCE=.\RMsg_JoinedParty.i
# End Source File
# Begin Source File

SOURCE=.\RMsg_LeaveRoom.i
# End Source File
# Begin Source File

SOURCE=.\RMsg_LoginAck.i
# End Source File
# Begin Source File

SOURCE=.\RMsg_Logout.i
# End Source File
# Begin Source File

SOURCE=.\RMsg_NewlyAwakened.i
# End Source File
# Begin Source File

SOURCE=.\RMsg_Party.i
# End Source File
# Begin Source File

SOURCE=.\RMsg_PartyInfo.i
# End Source File
# Begin Source File

SOURCE=.\RMsg_Ping.i
# End Source File
# Begin Source File

SOURCE=.\RMsg_PlayerMsg.i
# End Source File
# Begin Source File

SOURCE=.\RMsg_PlayerUpdate.i
# End Source File
# Begin Source File

SOURCE=.\RMsg_RcvAvatarDescription.i
# End Source File
# Begin Source File

SOURCE=.\RMsg_RoomLoginAck.i
# End Source File
# Begin Source File

SOURCE=.\RMsg_Speech.i
# End Source File
# Begin Source File

SOURCE=.\RMsg_Update.i
# End Source File
# End Group
# End Target
# End Project
